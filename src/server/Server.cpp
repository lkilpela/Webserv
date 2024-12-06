#include "Server.hpp"

void closeFds(std::vector<int> fds){ std::for_each(fds.begin(), fds.end(), [](int fd){close(fd);}); }

Server::~Server(){
	std::for_each(serverFds.begin(), serverFds.end(), [](int fd){close(fd);});
	}

Server::Server(const std::vector<int> ports) {
    for (int port : ports) {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)            
			throw std::runtime_error("Failed to create socket");
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	        throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        if (listen(serverFd, BACKLOG) < 0)
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
        serverFds.push_back(serverFd);
		pollData.push_back({serverFd, POLLIN, 0});
    }
}

void pollServers(Server& Servers){
	while (true)
}


int main() {
	std::vector<int> ports = {8080, 8081};
    try {
		Server servers(ports);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

