#include "Socket.hpp"

/*running the server
	-execute program
	-open another terminal and type in command:
		telnet localhost 8080
	- use GET request
		GET / HTTP/1.1
		Host: localhost
	- push enter twice
	 */
int main() {
    // Step 1: Create the socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }
    std::cout << "Server socket created" << std::endl;

    // Step 2: Define server address
    sockaddr_in address{};
    address.sin_family = AF_INET;                // Use IPv4
    address.sin_addr.s_addr = INADDR_ANY;        // Bind to all available interfaces
    address.sin_port = htons(PORT);              // Convert port to network byte order

    // Step 3: Bind the socket to an address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }
    std::cout << "Binding successful" << std::endl;

    // Step 4: Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }
    std::cout << "Listening on port " << PORT << std::endl;

	// need to poll loop here? To listen to the clients request and create new client_fd

    // Step 5: Accept incoming connections
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        perror("Accept failed");
        return -1;
    }
    std::cout << "Client connected" << std::endl;

    // Step 6: Read the client's message
    char buffer[1024] = {0};
    ssize_t valread = read(client_fd, buffer, sizeof(buffer));
    if (valread < 0) {
        perror("Read failed");
        return -1;
    }
    std::cout << "Received message: " << buffer << std::endl;

    // Step 7: Send a response to the client
    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    send(client_fd, response, strlen(response), 0);
    std::cout << "Response sent" << std::endl;

    // Step 8: Close the connection
    close(client_fd);
    std::cout << "Client connection closed" << std::endl;

    close(server_fd);
    std::cout << "Server socket closed" << std::endl;

    return 0;
}

#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

struct Server {
    std::vector<pollfd> _pollData;
};

int main() {
    Server Servers;

    // Create the listening socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up the address and bind the socket
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    // Add the listening socket to pollData
    pollfd server_pollfd = {server_fd, POLLIN, 0};
    Servers._pollData.push_back(server_pollfd);

    while (true) {
        // Call poll() to monitor sockets
        int activity = poll(Servers._pollData.data(), Servers._pollData.size(), -1);
        if (activity < 0) {
            perror("Poll error");
            break;
        }

        // Process events
        for (int i = 0; i < Servers._pollData.size(); i++) {
            if (Servers._pollData[i].revents & POLLIN) {
                // Handle readable events
                if (Servers._pollData[i].fd == server_fd) {
                    // Accept new connection
                    sockaddr_in client_addr{};
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

                    if (client_fd < 0) {
                        perror("Accept failed");
                        continue;
                    }

                    std::cout << "New client connected: " << client_fd << std::endl;

                    // Add client socket to pollData
                    pollfd client_pollfd = {client_fd, POLLIN | POLLOUT, 0};
                    Servers._pollData.push_back(client_pollfd);
                } else {
                    // Read data from client socket
                    char buffer[1024];
                    int client_fd = Servers._pollData[i].fd;
                    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

                    if (bytes_read <= 0) {
                        if (bytes_read == 0) {
                            std::cout << "Client disconnected: " << client_fd << std::endl;
                        } else {
                            perror("Recv failed");
                        }

                        // Remove client socket from pollData
                        close(client_fd);
                        Servers._pollData.erase(Servers._pollData.begin() + i);
                        --i; // Adjust index after removal
                    } else {
                        std::cout << "Received from client " << client_fd << ": " << std::string(buffer, bytes_read) << std::endl;
                    }
                }
            } else if (Servers._pollData[i].revents & POLLOUT) {
                // Handle writable events (e.g., send response)
                int client_fd = Servers._pollData[i].fd;
                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                send(client_fd, response.c_str(), response.size(), 0);

                // Remove POLLOUT from events if no more data to send
                Servers._pollData[i].events &= ~POLLOUT;
            }
        }
    }

    // Clean up
    for (auto& pfd : Servers._pollData) {
        close(pfd.fd);
    }
    return 0;
}

