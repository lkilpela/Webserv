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

#define PORT 8080
#define BACKLOG 10

int createListeningSocket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening on port " << port << std::endl;
    return server_fd;
}

int main() {
    // Create the listening socket
    int server_fd = createListeningSocket(PORT);

    // Vector to store pollfd structs
    std::vector<pollfd> poll_fds;

    // Add the listening socket to poll_fds
    pollfd server_pollfd = {server_fd, POLLIN, 0};
    poll_fds.push_back(server_pollfd);

    // Vector to store client sockets
    std::vector<int> client_sockets;

    while (true) {
        // Call poll() to monitor file descriptors
        int activity = poll(poll_fds.data(), poll_fds.size(), -1);
        if (activity < 0) {
            perror("Poll error");
            break;
        }

        // Iterate through poll_fds
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                // If the listening socket is readable, accept a new connection
                if (poll_fds[i].fd == server_fd) {
                    sockaddr_in client_addr{};
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
                    if (client_fd < 0) {
                        perror("Accept failed");
                        continue;
                    }
                    std::cout << "New client connected: " << client_fd << std::endl;

                    // Add the new client socket to poll_fds
                    pollfd client_pollfd = {client_fd, POLLIN, 0};
                    poll_fds.push_back(client_pollfd);

                    // Add the new client socket to client_sockets
                    client_sockets.push_back(client_fd);
                }
                // If a client socket is readable, handle the client
                else {
                    char buffer[1024];
                    int client_fd = poll_fds[i].fd;

                    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
                    if (bytes_read <= 0) {
                        // Connection closed or error
                        if (bytes_read == 0) {
                            std::cout << "Client disconnected: " << client_fd << std::endl;
                        } else {
                            perror("Recv failed");
                        }

                        close(client_fd);

                        // Remove the client socket from poll_fds and client_sockets
                        poll_fds.erase(poll_fds.begin() + i);
                        client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_fd), client_sockets.end());

                        --i; // Adjust index due to removal
                    } else {
                        // Echo data back to client
                        std::string message(buffer, bytes_read);
                        std::cout << "Received from client " << client_fd << ": " << message << std::endl;

                        send(client_fd, message.c_str(), message.size(), 0);
                    }
                }
            }
        }
    }

    // Clean up
    for (int fd : client_sockets) {
        close(fd);
    }
    close(server_fd);

    return 0;
}
