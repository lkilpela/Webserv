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
		_pollData.push_back({serverFd, POLLIN, 0});
    }
}

void Server::listen(){
    // for (int i = 0; i < Servers.pollData.size(); i++)
    //     std::cout << Servers.pollData[i].fd << " " << Servers.serverFds[i] << std::endl;
	// std::for_each(_serverFds.begin(), _serverFds.end(), [](int fd){close(fd);});
	while (true){
        if (::poll(_pollData.data(), _pollData.size(), 0) == -1)
            throw std::runtime_error("Poll failed");
        for (unsigned long i = 0; i < _pollData.size(); i++){
			//##Accepts and creates new client
            if (_pollData[i].revents & POLLIN){
				if (utils::isInVector(_pollData[i].fd, _serverFds)){
					sockaddr_in clientAddr {};
					socklen_t addrLen = sizeof(clientAddr);
					int clientFd = ::accept(_pollData[i].fd, (struct sockaddr*)&clientAddr, &addrLen);
					if (clientFd < 0) {
						std::cerr << "accept failed" << std::endl;
						continue;
					}
					pollfd clientPollData {clientFd, POLLIN | POLLOUT, 0};
					_pollData.push_back(clientPollData);
				}				
			}
			else if(_pollData[i].revents & POLLOUT){
				//timeout
				//http::Request request(clientFd);
				//http::Response response(clientFd);
				// process(request, response); CGI in here 
			}
        }       
    }
}

Server::~Server(){
	std::for_each(_serverFds.begin(), _serverFds.end(), [](int fd){close(fd);});
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

