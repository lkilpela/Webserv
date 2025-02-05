#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

Server::Server(int fd, const ServerConfig& serverConfig)
	: _fd(fd)
	, _serverConfig(serverConfig)
	, _router(serverConfig) {
}

int Server::addConnection() {
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = ::accept(_fd, (struct sockaddr*)&clientAddr, &addrLen);

	if (clientFd < 0) {
		perror("Failed to accept connection");
	} else {
		_connections.emplace(clientFd, http::Connection(clientFd, _serverConfig));
	}

	return clientFd;
}

void Server::closeConnection(int fd) {
	_connections.at(fd).close();
}

void Server::removeConnection(int fd) {
	auto it = _connections.find(fd);

	if (it != _connections.end()) {
		_connections.erase(it);
	}
}

void Server::process(int fd, short& events) {
	// auto& con = _connections.at(fd);
	(void)fd;
	(void) events;

}

void Server::sendResponse(int fd, short& events) {
	auto& con = _connections.at(fd);

	if (con.isClosed()) {
		return;
	}

	if (con.sendResponse()) {
		events &= ~POLLOUT;
	}
}

int Server::getServerFd() const {
	return _fd;
}

std::unordered_map<int, http::Connection>& Server::getAllConnections() {
	return _connections;
}

// char **makeEnv(char** &env, http::Request& req){

// 	http::Url url = req.getUrl();
// 	char **res = new char*[9];

// 	res[0] = new char[url.scheme.size() + 1];
// 	res[1] = new char[url.user.size() + 1];
// 	res[2] = new char[url.password.size() + 1];
// 	res[3] = new char[url.host.size() + 1];
// 	res[4] = new char[url.port.size() + 1];
// 	res[5] = new char[url.path.size() + 1];
// 	res[6] = new char[url.query.size() + 1];
// 	res[7] = new char[url.fragment.size() + 1];
// 	res[8] = nullptr;

// 	return res;
// }

// void deleteEnv(char **env){
// 	for (int i = 0; env[i]; i++){
// 		delete[] env[i];
// 	}
// 	delete[] env;
// }

// void Server::_cgiHandler(http::Request &req, http::Response &res) {

// 	char* interpreter = "/usr/bin/python3";
// 	// if (access(interpreter, X_OK) == -1 || access(script, X_OK) == -1){}
// 		//return 403 forbidding
// 	const char* script = req.getUrl().path.c_str();
// 	char* scriptArray[2] = {const_cast<char*>(script), nullptr};
// 	int pipefd[2];
// 	if(pipe(pipefd) == -1)
// 		perror("Pipe failed");
// 	if (utils::setNonBlocking(pipefd[0]) == false)
// 		perror("Pipe nonblocking failed");
// 	pollfd cgiData { pipefd[0], POLLIN, 0 };
// 	_pollfds.push_back(cgiData);
// 	pid_t pid = fork();
// 	if (pid == 0){
// 		close(pipefd[0]);
// 		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
// 			perror("Dup2 failed");
// 		char **env = makeEnv(env, req);
// 		execve(interpreter, scriptArray, env);
// 		deleteEnv(env);
// 	}
// 	pid_t result = waitpid(pid, NULL, WNOHANG);
// 	if (result = pid)
// 		close(pipefd[0]);
// }

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

void Server::_cleanup() {
	for (auto& [fd, con] : _connections) {
		con.close();
	}
}

Server::~Server() {
	_cleanup();
}
