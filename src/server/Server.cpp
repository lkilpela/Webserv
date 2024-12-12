#include "Server.hpp"
#include "Utils.hpp"

using std::cout;
using std::endl;

void Server::start() {
	std::cout << "Starting server at " << host << ":" << port << std::endl;
}

int setNonBlocking(int fd) {
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag == -1)
		return -1;
	return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

Server::Server(const std::vector<int> ports) {
    for (int port : ports) {
        std::cout << "port: " << port << std::endl;
        int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)            
			throw std::runtime_error("Failed to create socket");
		if (setNonBlocking(serverFd) == -1)
			throw std::runtime_error("Failed to create nonblocking");
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (::bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	        throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        if (::listen(serverFd, BACKLOG) == -1)
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
        _serverFds.push_back(serverFd);
		_pollData.push_back({serverFd, POLLIN, 0});
    }
}

void Server::listen(){
    // for (int i = 0; i < Servers.pollData.size(); i++)
    //     std::cout << Servers.pollData[i].fd << " " << Servers.serverFds[i] << std::endl;
	// std::for_each(_serverFds.begin(), _serverFds.end(), [](int fd){close(fd);});

	std::map<int, std::string> clientRequests;

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
					if (clientFd == -1) {
						std::cerr << "accept failed" << std::endl;
						continue;
					}
					pollfd clientPollData {clientFd, POLLIN, 0};
					_pollData.push_back(clientPollData);
				} else {
/* 					//handle client request
					char buffer[4096];
                    ssize_t bytes_read = recv(_pollData[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes_read <= 0) {
                        close(_pollData[i].fd);
                        _pollData.erase(_pollData.begin() + i);
                        --i;
                    } else {
                        buffer[bytes_read] = '\0';
                        clientRequests[_pollData[i].fd] = buffer;
                        _pollData[i].events = POLLOUT; // Prepare for response
                    } */
                }			
			} else if(_pollData[i].revents & POLLOUT){
/* 				// Send response
                const std::string& request = clientRequests[_pollData[i].fd];
                std::string response = handleRequest(request, config);
                send(_pollData[i].fd, response.c_str(), response.size(), 0);
                close(_pollData[i].fd);
                _pollData.erase(_pollData.begin() + i);
                --i; */

			}
        }       
    }
}

Server::~Server(){
	std::for_each(_serverFds.begin(), _serverFds.end(), [](int fd){close(fd);});
	}

// int main() {
// 	std::vector<int> ports = {8080, 8081};
//     try {
// 		Server server(ports);
//         server.listen();
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }

/* void Server::handleRequest(const Request& request, const Config& config) {
    // Find the server that matches the request host and port
    for (const auto& server : config.servers) {
        if (server.host == request.host && server.port == request.port) {
            // Find the location that matches the request path
            for (const auto& location : server.locations) {
                if (request.path.find(location.path) == 0) {
                    // Check if the method is allowed
                    if (find(location.methods.begin(), location.methods.end(), request.method) != location.methods.end()) {
                        cout << "Handling request for path: " << request.path << endl;
                        cout << "Root directory: " << location.root << endl;
                        cout << "Index file: " << location.index << endl;
                        cout << "Autoindex: " << (location.isAutoIndex ? "on" : "off") << endl;
                        return;
                    } else {
                        cout << "Method not allowed" << endl;
                        return;
                    }
                }
            }
            cout << "Path not found" << endl;
            return;
        }
    }
    cout << "Server not found" << endl;
} */
