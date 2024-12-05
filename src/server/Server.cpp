#include "Server.hpp"

void closeSockets(std::vector<int> severFd){
    std::for_each(severFd.begin(), severFd.end(), [](int fd) {close(fd);});
}

std::vector<int> makeSocket(const std::vector<int>& ports) {
    std::vector<int> sockets;
    for (int port : ports) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
			closeSockets(sockets);
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
            closeSockets(sockets);
            throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
        }
        sockets.push_back(sock);
    }
    return sockets;
}

int main() {

	Server sockets;
    try {
        std::vector<int> socketsArray = makeSocket(sockets.getPorts());
        for (int sock : socketsArray) {
            close(sock);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

