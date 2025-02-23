#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

using enum WorkerProcess::Status;

Server::Server(const ServerConfig& serverConfig) : _serverConfig(serverConfig) , _router(serverConfig) {
	_serverFds.reserve(serverConfig.ports.size());

	for (const int port : serverConfig.ports) {
		int serverFd = utils::createPassiveSocket(serverConfig.host.data(), port, 128, true);
		std::cout << "listening on " << serverConfig.host << ":" << port << std::endl;
		_serverFds.emplace(serverFd);
	}

	_router.get(handleGetRequest);
	_router.post(handlePostRequest);
	_router.del(handleDeleteRequest);

	// _router.setCgiHandler([this](const Location& loc, const std::string& requestPath, http::Request& req, http::Response& res) {
	// 	std::cout << "this->_workerProcesses.size()=" << this->_workerProcesses.size() << std::endl;
	// 	this->_handleCGI(loc, requestPath, req, res);
	// });
}

void Server::addCgiHandler() {
	_router.setCgiHandler([this](const Location& loc, const std::string& requestPath, http::Request& req, http::Response& res) {
		std::cout << "this->_workerProcesses.size()=" << this->_workerProcesses.size() << std::endl;
		this->_handleCGI(loc, requestPath, req, res);
	});
}

void Server::addConnection(int serverFd) {
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);

	int clientFd = ::accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);

	if (clientFd >= 0) {
		_connections.emplace(clientFd, http::Connection(clientFd, _serverConfig));
	}
}

void Server::closeConnection(http::Connection& con) {
	int clientFd = con.getClientFd();
	con.close();

	for (auto& [_, process] : _workerProcesses) {
		if (process.clientFd == clientFd && process.status == RUNNING) {
			::kill(process.pid, SIGTERM);
			::close(process.pipeFds[0]);
			process.pipeFds[0] = -1;
			process.status = PENDING_TERMINATION;
			pid_t pid = ::waitpid(process.pid, NULL, WNOHANG);

			if (pid > 0 || (pid == -1 && errno == ECHILD)) {
				process.status = TERMINATED;
			}
		}
	}
}

void Server::close(int fd) {
	(void)fd;
}

void Server::process(int fd, short& events) {
	std::cout << "process() fd = "<< fd << std::endl;
	if (auto it = _connections.find(fd); it != _connections.end()) {
		return _processConnection(it->second, events);
	}

	if (auto it = _workerProcesses.find(fd); it != _workerProcesses.end()) {
		return _processWorkerProcess(it->second, events);
	}
}

void Server::sendResponse(int fd, short& events) {
	auto it = _connections.find(fd);

	if (it != _connections.end() && it->second.sendResponse()) {
		events &= ~POLLOUT;
	}
}

const std::unordered_set<int>& Server::getServerFds() const {
	return _serverFds;
}

std::unordered_map<int, http::Connection>& Server::getManagedConnections() {
	return _connections;
}

std::unordered_map<int, WorkerProcess>& Server::getWorkerProcesses() {
	return _workerProcesses;
}

void Server::_processConnection(http::Connection& con, short& events) {
	con.read();
	auto* req = con.getRequest();
	auto* res = con.getResponse();

	if (req == nullptr || res == nullptr) {
		return;
	}

	using enum http::Response::Status;

	if (res->getStatus() == PENDING) {
		res->setStatus(IN_PROGRESS);
		std::cout << "_processConnection()" << con.getClientFd() << std::endl;
		_router.handle(*req, *res);

		if (res->getStatus() == READY) {
			events |= POLLOUT;
		}
	}
}

void Server::_processWorkerProcess(WorkerProcess& process, short& events) {
	auto it = _connections.find(process.clientFd);

	if (it == _connections.end() || it->second.isClosed()) {
		return;
	}

	http::Response* res = it->second.getResponse();

	if (res == nullptr || res->getStatus() == http::Response::Status::READY) {
		return;
	}

	unsigned char buffer[4096];
	ssize_t bytesRead = ::read(process.pipeFds[0], buffer, sizeof(buffer));

	if (bytesRead <= 0) {
		if (bytesRead == 0) {
			res->setStatusCode(http::StatusCode::OK_200).build();
			std::cout << "bytesRead == 0" << std::endl;
		}

		if (bytesRead < 0) {
			res->clear().setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, process.rootPath / "500.html");
		}

		events |= POLLOUT;
		::close(process.pipeFds[0]);
		process.pipeFds[0] = -1;
		process.status = TERMINATED;
		return;
	}

	if (res->getBody() == nullptr) {
		res->setBody(std::make_unique<utils::StringPayload>(res->getClientSocket(), ""));
	}

	res->appendBody(buffer, bytesRead);
	std::cout << res->getBody()->toString() << std::endl;
}

// void Server::_closePipeFd(int fd) {
// 	auto it = _workerProcesses.find(fd);

// 	if (it != _workerProcesses.end()) {
// 		auto& process = it->second;

// 		if (!process.isPipeClosed) {
// 			pid_t pid;

// 			pid = ::waitpid(process.pid, NULL, WNOHANG);

// 			if (pid > 0) {
// 				::close(fd);
// 				process.pipeFd = -1;
// 				process.isPipeClosed = true;
// 				return;
// 			}

// 			::kill(process.pid, SIGTERM);
// 		}
// 	}
// }

void Server::_handleCGI(
	const Location& loc,
	const std::string& requestPath,
	const http::Request& request,
	http::Response& response
) {
	std::cout << YELLOW "Handling CGI request" RESET << std::endl;
	// (void)loc;
	// (void)requestPath;
	(void)request;
	(void)response;
	std::string scriptPath = loc.root / requestPath.substr(loc.path.size());
	std::cout << "scriptPath=" << scriptPath << std::endl;
	// Validate CGI script
	// if (!fs::exists(scriptPath) || !fs::is_regular_file(scriptPath)) {
	// 	response.setFile(StatusCode::NOT_FOUND_404, loc.root / "404.html");
	// 	return;
	// }

	WorkerProcess process;

	process.clientFd = response.getClientSocket();
	process.rootPath = loc.root;

	if (::pipe(process.pipeFds) == -1) {
		response.setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
		return;
	}

	process.pid = ::fork();

	if (process.pid == -1) {
		response.setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
		return;
	}

	if (process.pid == 0) {
		::close(process.pipeFds[0]);
		::dup2(process.pipeFds[1], STDOUT_FILENO);
		::close(process.pipeFds[1]);

		// auto envp = request.getCgiEnvp();
		std::string interpreter("/usr/bin/python3");
		char* argv[] = { interpreter.data(), scriptPath.data(), NULL };

		::execve(argv[0], argv, NULL);
		std::cerr << "Error execve()";
		::exit(1);
	}

	::close(process.pipeFds[1]);
	process.pipeFds[1] = -1;
	_workerProcesses.emplace(process.pipeFds[0], process);
	std::cout << "Read-End pipeFd " << process.pipeFds[0] << " has been added to pollfds" << std::endl;
}

Server::~Server() {
	_cleanup();
}

void Server::_cleanup() {
	for (auto& [fd, con] : _connections) {
		con.close();
	}
}

void Server::shutdown() {
	// auto cgiProcesses = getWorkerProcesses();

	// for (auto& child : cgiProcesses){
	// 	kill(child.second.pid, SIGINT);
	// 	waitpid(child.second.pid, nullptr, 0);
	// 	child.second.isPipeClosed = true;
	// 	close(child.second.pipeFd);
	// }
}
