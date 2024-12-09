#include "Server.hpp"
#include "Utils.hpp"

Servers::Servers(const std::vector<int> ports) {
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

// template <typename T>
// 	bool isInVector(T element, std::vector<T>& v){
// 		return std::find(v.begin(), v.end(), element) != v.end();
// 	}

void Servers::pollServers(Servers& Servers){
    // for (int i = 0; i < Servers.pollData.size(); i++)
    //     std::cout << Servers.pollData[i].fd << " " << Servers.serverFds[i] << std::endl;
	while (true){
        if (poll(Servers.pollData.data(), Servers.pollData.size(), 0) == -1)
            throw std::runtime_error("Poll failed");
        for (unsigned long i = 0; i < Servers.pollData.size(); i++){
			//Accepts and creates new client
            if (Servers.pollData[i].revents & POLLIN){
				if (utils::isInVector(Servers.pollData[i].fd, Servers.serverFds)){
				// if (isInVector(Servers.pollData[i].fd, Servers.serverFds)){
					sockaddr_in clientAddr {};
					socklen_t addrLen = sizeof(clientAddr);
					int clientFd = accept(Servers.pollData[i].fd, (struct sockaddr*)&clientAddr, &addrLen);
					if (clientFd < 0){
						
						continue;
					}
				}
			}
			// else(Servers.pollData[i].revents & POLLOUT){
			// 	//read
			// }
        }
            
    }
}

Servers::~Servers(){
	std::for_each(serverFds.begin(), serverFds.end(), [](int fd){close(fd);});
	}

int main() {
	std::vector<int> ports = {8080, 8081};
    try {
		Servers servers(ports);
        servers.pollServers(servers);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

