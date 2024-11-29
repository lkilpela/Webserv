#include "Socket.hpp"

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
