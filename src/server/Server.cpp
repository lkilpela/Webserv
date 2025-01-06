#include "Server.hpp"
#include "http/Connection.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"
#include "../../include/http/Connection.hpp"
#include "../../include/http/Request.hpp"
#include <unistd.h>

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
		return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return -1;
	return 0;
}

Server::Server(const Config& config) {
    for (int port : config.ports) {
        int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)
			throw std::runtime_error("Failed to create socket");
		if (setNonBlocking(serverFd) == -1)
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

void Server::listen() {
	while (_pollfds.size()) {
		if (sigintReceived){
			utils::closeFDs(_clientFds);
			utils::closeFDs(_serverFds);
			break;
		}

        if (::poll(_pollfds.data(), _pollfds.size(), 50) == -1)
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
					_connectionByFd.emplace(it->fd, connection);
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

char **makeEnv(char** &env, http::Request& req){

	http::Url url = req.getUrl();
	char **res = new char*[9];

	res[0] = new char[url.scheme.size() + 1];
	res[1] = new char[url.user.size() + 1];
	res[2] = new char[url.password.size() + 1];
	res[3] = new char[url.host.size() + 1];
	res[4] = new char[url.port.size() + 1];
	res[5] = new char[url.path.size() + 1];
	res[6] = new char[url.query.size() + 1];
	res[7] = new char[url.fragment.size() + 1];
	res[8] = nullptr;

	return res;
}

void deleteEnv(char **env){
	for (int i = 0; env[i]; i++){
		delete[] env[i];
	}
	delete[] env;
}
// send reponse of internal error if execve fails?
// if (req.getUrl().path ends with "abc.py") {
// this is CGI request then do something with it
void Server::_addConnection(int fd) {
	//if (CGI){
		char* interpreter = "/usr/bin/python3";
		char* script = "cgi_bin/hello.py";
		char*scriptArray[2] = {script, nullptr};
		if (access(interpreter, X_OK) == -1 || access(script, X_OK) == -1){}
			//return 403 forbidding
		char **env;
		auto CgiHandle = [&](http::Request& req, http::Response& res) -> void {
			int pipefd[2];
			if(pipe(pipefd) == -1)
				perror("Pipe failed");
			if (setNonBlocking(pipefd[0]) == -1)
				perror("Pipe nonblocking failed");
			pollfd cgiData { pipefd[0], POLLIN, 0 };
			_pollfds.push_back(cgiData);
			pid_t pid = fork();
			if (pid == 0){
				close(pipefd[0]);
				if (dup2(pipefd[1], STDOUT_FILENO) == -1)
					perror("Dup2 failed");
				env = makeEnv(env, req);
				execve(interpreter, scriptArray, env);
			} else{
				deleteEnv(env);
				close(pipefd[1]);
			}
			//waitpid
		};
		http::Connection connection(fd, 5000, std::function<void(http::Request&, http::Response&)>(CgiHandle));
	//}
			
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = ::accept(fd, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientFd < 0) {
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