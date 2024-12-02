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
	std::cout << "here" << std::endl;
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

/*
1. Networking and Connection Handling
Responsibilities:
Implement the socket creation, binding, listening, and accepting client connections.
Handle non-blocking I/O using poll() or an equivalent mechanism.
Manage multiple clients simultaneously.
Key Functions: socket(), bind(), listen(), accept(), poll().
Who Should Take This?
A student with an interest in low-level programming or networking concepts.

2. HTTP Request and Response Handling
Responsibilities:
Parse HTTP requests: Extract method, URL, headers, and body.
Build HTTP responses: Status line, headers, and body.
Handle error responses (e.g., 404 Not Found, 500 Internal Server Error).
Support HTTP methods (GET, POST, DELETE).
Key Functions: recv(), send().
Who Should Take This?
A student interested in parsing protocols and working with strings.

3. File Handling and Static Content
Responsibilities:
CGI
Serve static files (e.g., HTML, CSS, images).
Handle directory traversal and listing (if required).
Manage file uploads (write files to disk).
Implement configuration file parsing to set server parameters.
Key Functions: open(), read(), write(), opendir(), readdir(), close().
Who Should Take This?
A student comfortable working with file systems and configuration files.*/