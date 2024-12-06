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

#define PORT 8080
#define BACKLOG 128

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    // Set backlog size
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << " with a backlog of " << BACKLOG << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            if (errno == ECONNABORTED || errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "Connection dropped (backlog full or client disconnected)" << std::endl;
                continue;
            }
            perror("Accept failed");
            break;
        }

        std::cout << "New client connected!" << std::endl;

        // Simulate handling client and close connection
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
