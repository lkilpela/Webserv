#include "Server.hpp"
#include "Utils.hpp"

Server::Server(const std::vector<int> ports) {
    for (int port : ports) {
        int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)
			throw std::runtime_error("Failed to create socket");
		int flag = fcntl(serverFd, F_GETFL, 0);
		if (flag == -1 || fcntl(serverFd, F_SETFL, flag | O_NONBLOCK) == -1)
			throw std::runtime_error("Failed to create nonblocking");
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = ::htons(port);
        if (::bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	        throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        if (::listen(serverFd, BACKLOG) < 0)
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
        _serverFds.push_back(serverFd);
		_pollfds.push_back({serverFd, POLLIN, 0});
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

void Server::process(Request& req, Response& res) {

}

void Server::listen(){
	while (true){
        if (::poll(_pollfds.data(), _pollfds.size(), 0) == -1)
            throw std::runtime_error("Poll failed");
        for (std::size_t i = 0; i < _pollfds.size(); i++) {
			//##Accepts and creates new client
            if (_pollfds[i].revents & POLLIN) {
				if (utils::isInVector(_pollfds[i].fd, _serverFds)){
					_addClient(i);
				} else {
					const int fd = _pollfds[i].fd;
					unsigned char buffer[1024];
					ssize_t bytesRead = recv(fd, buffer, 1024, 0);

					auto& [req, res] = _requestResponseByFd.at(fd);
					req.append(buffer);
					if (req.isComplete()) {
						req.parse();
						process(req, res);
						if (res.isReady())
						_pollfds[i].events |= POLLOUT;
					}
				}
			}
			else if(_pollfds[i].revents & POLLOUT) {
				//timeout
				//http::Request request(clientFd);
				//http::Response response(clientFd);
				// process(request, response); CGI in here
			}
        }
    }
}

Server::~Server() {
	std::for_each(
		_serverFds.begin(),
		_serverFds.end(),
		[](int fd){ close(fd); }
	);
}

int main() {
	std::vector<int> ports = {8080, 8081};
    try {
		Server server(ports);
        server.listen();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

