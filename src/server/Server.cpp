#include "Server.hpp"
#include "Utils.hpp"


Server::Server(const Config& config) {
	//need to figure out route mapping
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

void Server::processHttpClient(Request& req, Response& res) {

}

void Server::_handleSigInt(int sig){
	utils::closeFDs(_serverFds);
	utils::closeFDs(_clientFds);
	std::cout << "done" << std::endl;
}

void Server::_handleSignals(){
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, [](int sig) {
        if (_serverInstance) {
            _serverInstance->_handleSigInt(sig);
        }
    });
}

void Server::listen() {
	while (true) {
        if (::poll(_pollfds.data(), _pollfds.size(), 0) == -1)
			perror("Poll failed");
        for (std::size_t i = 0; i < _pollfds.size(); i++) {
			const int fd = _pollfds[i].fd;			
			if (_isNewClient(fd)) {
				_addClient(fd);
			} else {
				auto& [req, res] = _requestResponseByFd.at(fd);
				if (_isConnectedClient(fd))
					processHttpClient(req, res);					
				else
					processCGI(req);
			}
		}
    }
}

void Server::_addClient(std::size_t i) {
	sockaddr_in clientAddr {};
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = ::accept(_pollfds[i].fd, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientFd < 0) {
		// May use std::cerr for logging error
		perror("Failed to accept connection");
		return;
	}
	pollfd clientPollData { clientFd, POLLIN, 0 };
	_clientFds.push_back(clientFd);
	_pollfds.push_back(clientPollData);
}

bool Server::_isNewClient(int fd) const {
	return utils::isInVector<int>(fd, _serverFds);
}

bool Server::_isConnectedClient(int fd) const {
	return utils::isInVector<int>(fd, _clientFds);
}

Server::~Server() {
	utils::closeFDs(_serverFds);
}

/* int main() {
	std::vector<int> ports = {8080, 8081};
    try {
		Server server(ports);
        server.listen();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} */

