#include <sys/socket.h>
#include <netinet/in.h>
#include "Server.hpp"
#include "utils/index.hpp"
#include "SignalHandle.hpp"

Server::Server(const Config& config) : _config(config), _router(config.servers[0]) {
    for (int port : config.ports) {
		int serverFd = utils::createPassiveSocket(port, 128, true);
        _serverFds.emplace(serverFd);
		_pollfds.push_back({ serverFd, POLLIN, 0 });
    }
}

void Server::listen() {
	while (_pollfds.size()) {
		if (sigintReceived){
			_cleanup();
			break;
		}

        if (::poll(_pollfds.data(), _pollfds.size(), 50) == -1)
			perror("Poll failed");

        for (auto it = _pollfds.begin(); it != _pollfds.end();) {
			const auto [fd, events, revents] = *it; // fd here can be server, client or pipe file descriptor

			// if fd is not serverFD then we execute code below
			if (_serverFds.find(fd) != _serverFds.end()) {
				if (revents & POLLIN) {
					_addConnection(fd);
				}
			} else {
				auto& connection = _connectionByFd[fd];

				if (revents & POLLHUP) {
					connection.close();
				} else {
					_read(*it, connection);
					_process(*it, connection);
					_sendResponse(*it, connection);
				}

				if (connection.isClosed()) {
					_connectionByFd.erase(it->fd);
					it = _pollfds.erase(it);
					continue;
				}
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

// void Server::cgiHandler(http::Request &req, http::Response &res) {

// 		char* interpreter = "/usr/bin/python3";
// 		char* script = "cgi_bin/hello.py";
// 		char* scriptArray[2] = {script, nullptr};
// 		if (access(interpreter, X_OK) == -1 || access(script, X_OK) == -1){}
// 			//return 403 forbidding
// 		auto cgiHandler = [&](http::Request& req, http::Response& res) -> void {
// 			int pipefd[2];
// 			if(pipe(pipefd) == -1)
// 				perror("Pipe failed");
// 			if (utils::setNonBlocking(pipefd[0]) == false)
// 				perror("Pipe nonblocking failed");
// 			pollfd cgiData { pipefd[0], POLLIN, 0 };
// 			_pollfds.push_back(cgiData);
// 			pid_t pid = fork();
// 			if (pid == 0){
// 				close(pipefd[0]);
// 				if (dup2(pipefd[1], STDOUT_FILENO) == -1)
// 					perror("Dup2 failed");
// 				char **env = makeEnv(env, req);
// 				execve(interpreter, scriptArray, env);
// 			} else{
// 				deleteEnv(env);
// 				//read pipe and give to response
// 				close(pipefd[1]);
// 			}
// 			//waitpid(pid, &status, WNOHANG);
// 		};
// }
// send reponse of internal error if execve fails?
// if (req.getUrl().path ends with "abc.py") {
// this is CGI request then do something with it

void Server::_addConnection(int serverFd) {
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = ::accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);

	if (clientFd < 0) {
		perror("Failed to accept connection");
		return;
	}

	struct ::pollfd clientPollFd { clientFd, POLLIN, 0 };
	_pollfds.push_back(clientPollFd);
	_connectionByFd.emplace(clientFd, Connection(clientFd, ));
}

void Server::_read(struct ::pollfd& pollFd, http::Connection& con) {
	if (pollFd.revents & POLLIN == 0) {
		return;
	}

	if (pollFd.fd) {
		_readFromPipe(pollFd, con);
	} else {
		_readFromSocket(pollFd, con);
	}
}

void Server::_readFromPipe(struct ::pollfd& pollFd, http::Connection& con) {
	unsigned char buffer[4096];
	ssize_t bytesRead = ::read(pollFd.fd, buffer, sizeof(buffer));

	if (bytesRead < 0) {
		con.close();
		return;
	}

	http::Response* res = con.getResponse();
	if (res != nullptr) {
		res->getBody()->append(buffer, bytesRead);
	}
}

void Server::_readFromSocket(struct ::pollfd& pollFd, http::Connection& con) {
	char buffer[4096];
	ssize_t bytesRead = ::recv(pollFd.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (bytesRead == 0) {
		con.close();
		return;
	}

	if (bytesRead > 0) {
		con.append(buffer, bytesRead);
	}
}

void Server::_process(struct ::pollfd& pollFd, http::Connection& con) {
	if (con.isClosed()) {
		return;
	}

	using enum http::Response::Status;
	http::Request* req = con.getRequest();
	http::Response* res = con.getResponse();

	if (res == nullptr) {
		return;
	}

	if (res->getStatus() == PENDING) {
		res->setStatus(IN_PROGRESS);
		_router.handle(*req, *res);
	}
}

void Server::_sendResponse(struct ::pollfd& pollFd, http::Connection& con) {
	if (con.isClosed()) {
		return;
	}

	if (pollFd.revents & POLLOUT) {
		if (con.sendResponse()) {
			pollFd.events &= ~POLLOUT;
		}
	}
}

void Server::_cleanup() {
	for (auto &pollfd : _pollfds) {
		::close(pollfd.fd);
	}
}

Server::~Server() {
	_cleanup();
}
