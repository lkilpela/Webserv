#include "Server.hpp"
#include "http/Connection.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

Server::Server(const Config& config) {
    for (int port : config.ports) {
        int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)
			throw std::runtime_error("Failed to create socket");
		if (utils::setNonBlocking(serverFd) == -1)
			throw std::runtime_error("Nonblocking failed");
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (::bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	        throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        if (::listen(serverFd, BACKLOG) < 0)
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
        _serverFds.push_back(serverFd);
		_pollfds.push_back({serverFd, POLLIN, 0});
    }
}

void Server::processCGI(Request& req) {
	int pipefd[2];
	if(pipe(pipefd) == -1)
		perror("Pipe failed");
	if (utils::setNonBlocking(pipefd[0]) == -1)
		perror("Pipe nonblocking failed");
	pollfd cgiData { pipefd[0], POLLIN, 0 };
	_pollfds.push_back(cgiData);
	pid_t pid = fork();
	if (pid == 0){
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
			perror("Dup2 failed");
			//exceve
	} else
	close(pipefd[1]);
}

void Server::listen() {
	while (_pollfds.size()) {
		if (sigintReceived){
			utils::closeFDs(_clientFds);
			utils::closeFDs(_serverFds);
			break;
		}

        if (::poll(_pollfds.data(), _pollfds.size(), 0) == -1)
			perror("Poll failed");
        for (auto it = _pollfds.begin(); it != _pollfds.end();) {
			auto& connection = _connectionByFd[it->fd];
			if (connection.isTimedOut()) {
				_connectionByFd.erase(it->fd);
				it = _pollfds.erase(it);
				continue;
			}

			if (it->revents == POLLHUP) {
				connection.close();
				_connectionByFd.erase(it->fd);
				it = _pollfds.erase(it);
				continue;
			}

			if (it->revents == POLLIN) {
				if (utils::isInVector<int>(it->fd, _serverFds)) {
					_addConnection(it->fd);
					_connectionByFd.emplace(it->fd, );
				} else if (utils::isInVector<int>(it->fd, _clientFds)) {
					unsigned char buffer[2048];

					ssize_t bytesRead = recv(it->fd, buffer, 2048, MSG_NOSIGNAL);

					// client closed connection successfully
					if (bytesRead == 0) {
						connection.close();
						_connectionByFd.erase(it->fd);
						it = _pollfds.erase(it);
						continue;
					}

					connection.readRequest(buffer, bytesRead);
				}
			} else if (it->revents == POLLOUT) {
				connection.sendResponse();
			}
			it++;
		}
    }
}

void Server::_addConnection(int fd) {
	http::Connection connection(fd, 5000, [&](Request& req, Response& res) {
		// access server through this
		// access ServerConfig
		// if (req.getUrl().path ends with "abc.py") {
		//    this is CGI request then do something with it
		// }
	});
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = ::accept(fd, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientFd < 0) {
		// May use std::cerr for logging error
		perror("Failed to accept connection");
		return;
	}
	pollfd clientPollData { fd, POLLIN, 0 };
	_clientFds.push_back(clientFd);
	_pollfds.push_back(clientPollData);
	_connectionByFd.emplace(connection);
}



Server::~Server() {
	utils::closeFDs(_serverFds);
}

