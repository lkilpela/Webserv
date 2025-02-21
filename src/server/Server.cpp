#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

Server::Server(const ServerConfig& serverConfig) : _serverConfig(serverConfig), _router(serverConfig) {
	_router.get(handleGetRequest);
	_router.post(handlePostRequest);
	_router.del(handleDeleteRequest);

	_serverFds.reserve(serverConfig.ports.size());

	for (const int port : serverConfig.ports) {
		int serverFd = utils::createPassiveSocket(serverConfig.host.data(), port, 128, true);
		std::cout << "listening on " << serverConfig.host << ":" << port << std::endl;
		_serverFds.emplace(serverFd);
	}
}

void Server::addClientTo(int serverFd) {
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);

	int clientFd = ::accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);

	if (clientFd >= 0) {
		_connectionByClientFd.emplace(clientFd, http::Connection(clientFd, _serverConfig));
	}
}

void Server::close(int fd) {
	if (_processByPipeFd.find(fd) != _processByPipeFd.end()) {
		return _closePipeFd(fd);
	}

	auto it = _connectionByClientFd.find(fd);

	if (it == _connectionByClientFd.end() || it->second.isClosed()) {
		return;
	}

	for (auto& [pipeFd, process] : _processByPipeFd) {
		if (process.clientFd == fd) {
			_closePipeFd(pipeFd);
			if (process.isPipeClosed) {
				it->second.close();
			}
		}
	}
}

void Server::process(int fd, short& events) {
	auto& con = _connectionByClientFd.at(fd);

	con.read();

	auto* req = con.getRequest();
	auto* res = con.getResponse();

	if (req == nullptr || res == nullptr) {
		return;
	}

	using enum http::Response::Status;

	if (res->getStatus() == PENDING) {
		res->setStatus(IN_PROGRESS);
		std::cout << "Is chunked request? " << req->isChunkEncoding() << ", is BAD_REQUEST? " << (req->getStatus() == http::Request::Status::BAD) << std::endl;
		_router.handle(*req, *res);

		if (res->getStatus() == READY) {
			events |= POLLOUT;
		}
	}
}

void Server::sendResponse(int fd, short& events) {
	auto& con = _connectionByClientFd.at(fd);

	if (con.sendResponse()) {
		events &= ~POLLOUT;
	}
}

const std::unordered_set<int>& Server::getServerFds() const {
	return _serverFds;
}

std::unordered_map<int, http::Connection>& Server::getClients() {
	return _connectionByClientFd;
}

std::unordered_map<int, Process>& Server::getPipeProcess() {
	return _processByPipeFd;
}

void Server::_closePipeFd(int fd) {
	auto it = _processByPipeFd.find(fd);

	if (it != _processByPipeFd.end()) {
		auto& process = it->second;

		if (!process.isPipeClosed) {
			pid_t pid;

			pid = ::waitpid(process.pid, NULL, WNOHANG);

			if (pid > 0) {
				::close(fd);
				process.pipeFd = -1;
				process.isPipeClosed = true;
				return;
			}

			::kill(process.pid, SIGTERM);
		}
	}
}

std::vector<char*> makeEnv(http::Request& req){

	http::Url url = req.getUrl();
	std::vector<char*> res;

	res.push_back(const_cast<char*>(req.getUrl().scheme.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().user.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().password.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().host.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().port.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().path.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().query.c_str()));
	res.push_back(const_cast<char*>(req.getUrl().fragment.c_str()));
	res.push_back(nullptr);

	return res;
}

void Server::_cgiHandler(http::Request &req, http::Response &res) {

	char* interpreter = "/usr/bin/python3";
	// if (access(interpreter, X_OK) == -1 || access(script, X_OK) == -1){}
		//return 403 forbidding
	const char* script = req.getUrl().path.c_str();
	char* scriptArray[2] = {const_cast<char*>(script), nullptr};
	int pipefd[2];
	if(pipe(pipefd) == -1)
		perror("Pipe failed");
	if (utils::setNonBlocking(pipefd[0]) == false)
		perror("Pipe nonblocking failed");
	pollfd cgiData { pipefd[0], POLLIN, 0 };
	_pollfds.push_back(cgiData);
	pid_t pid = fork();
	if (pid == 0){
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
			perror("Dup2 failed");
		std::vector<char*> env = makeEnv(req);
		execve(interpreter, scriptArray, env.data());
	}
	pid_t result = waitpid(pid, NULL, WNOHANG);
	if (result = pid)
		close(pipefd[0]);
}

// void Server::_read(struct ::pollfd& pollFd, http::Connection& con) {
// 	if (pollFd.revents & POLLIN == 0) {
// 		return;
// 	}

// 	if (pollFd.fd) {
// 		_readFromPipe(pollFd, con);
// 	} else {
// 		_readFromSocket(pollFd, con);
// 	}
// }

// void Server::_readFromPipe(struct ::pollfd& pollFd, http::Connection& con) {
// 	unsigned char buffer[4096];
// 	ssize_t bytesRead = ::read(pollFd.fd, buffer, sizeof(buffer));

// 	if (bytesRead < 0) {
// 		con.close();
// 		return;
// 	}

// 	http::Response* res = con.getResponse();
// 	if (res != nullptr) {
// 		res->getBody()->append(buffer, bytesRead);
// 	}
// }

// void Server::_readFromSocket(struct ::pollfd& pollFd, http::Connection& con) {
// 	char buffer[4096];
// 	ssize_t bytesRead = ::recv(pollFd.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

// 	if (bytesRead == 0) {
// 		con.close();
// 		return;
// 	}

// 	if (bytesRead > 0) {
// 		con.append(buffer, bytesRead);
// 	}
// }

// void Server::_handle(struct ::pollfd& pollFd, http::Connection& con) {
// 	using enum http::Response::Status;
// 	http::Request* req = con.getRequest();
// 	http::Response* res = con.getResponse();

// 	if (res == nullptr) {
// 		return;
// 	}

// 	if (res->getStatus() == PENDING) {
// 		res->setStatus(IN_PROGRESS);
// 		_router.handle(*req, *res);
// 	}
// }


Server::~Server() {
	_cleanup();
}

void Server::_cleanup() {
	for (auto& [fd, con] : _connectionByClientFd) {
		con.close();
	}
}

void Server::_shutDownServer(){

	auto cgiProcesses = getPipeProcess();

	for (auto& child : cgiProcesses){
		kill(child.second.pid, SIGINT);
		waitpid(child.second.pid, nullptr, 0);
		child.second.isPipeClosed = true;
		close(child.second.pipeFd);
	}
}
