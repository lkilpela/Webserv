#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include "Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "utils/common.hpp"
#include "http/Url.hpp"
#include "Server.hpp"
#include <exception>
#include <thread>
#include "SignalHandle.hpp"
#include "http/index.hpp"
#include "http/Connection.hpp"


volatile sig_atomic_t sigintReceived = 0;

// Function to read a binary file into a vector of bytes
std::vector<uint8_t> readBinaryFile(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}
	return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		//handleSignals();
		ConfigParser parser(argv[1]);
		Config config = parser.load();

/* 		// Create a router for each server
		ServerConfig serverConfig = config.servers[1];
		Router router(serverConfig);
		router.get(handleGetRequest);
		router.post(handlePostRequest);
		router.del(handleDeleteRequest);

		// Simulate Connection
        int clientSocket = 0; // Dummy socket for testing
        http::Connection connection(clientSocket, serverConfig, [&](http::Request& req, http::Response& res) {
            router.handle(req, res);
        });
 */
		// Read the image file
        //std::vector<uint8_t> imageData = readBinaryFile("config/http/uploads/image.jpg");

        // Simulate reading request
        //std::string buffer = "GET /cgi-bin/script.cgi/some/extra/path HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
		//std::string buffer = "GET /uploads/upload_file.txt HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
		//std::string buffer = "GET /uploads/image.jpg HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
		//std::string buffer = "GET /redirect HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
/*         std::string buffer = "POST /uploads/upload_file.txt HTTP/1.1\r\n"  // Request line
							"Host: localhost:8080\r\n" // Host header
							"Content-Length: 17\r\n" // Content-Length header
							"Content-Type: text/plain\r\n\r\n" // Content-Type header
							"Test POST request"; // Request body

		// Request to upload a image file
		std::string buffer = "POST /uploads/image.jpg HTTP/1.1\r\n"  // Request line
							"Host: localhost:8080\r\n" // Host header
							"Content-Length: 17\r\n" // Content-Length header
							"Content-Type: image/jpeg\r\n\r\n" // Content-Type header
							"Test POST request"; // Request body  */

/*         std::string buffer = "POST /uploads/image.jpg HTTP/1.1\r\n"
                             "Host: localhost:8080\r\n"
                             "Content-Length: " + std::to_string(imageData.size()) + "\r\n"
                             "Content-Type: image/jpeg\r\n\r\n";
        buffer.append(reinterpret_cast<const char*>(imageData.data()), imageData.size());

		// Request to delete a file
 		std::string buffer = "DELETE /uploads/image.jpg HTTP/1.1\r\n"  // Request line
							"Host: localhost:8080\r\n" // Host header
							"Content-Length: 0\r\n" // Content-Length header
							"Content-Type: image/jpeg\r\n\r\n"; // Content-Type header */

/* 		std::cout << YELLOW "Raw request header: \n" RESET << buffer << std::endl;
		connection.readRequest((uint8_t*)buffer.data(), buffer.size());

        // Send response
        connection.sendResponse(); */
/* 		// For Testing - Log the response status code
		std::cout << GREEN "Response status code: " RESET << static_cast<int>(res.getStatusCode()) << std::endl;
		std::cout << GREEN "Header: " RESET << res.getHeader().toString() << std::endl; */

		//Server server(config);
		//server.listen();
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.what() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

