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

// Function to simulate a request
void simulateRequest(Router& router, http::Request& request) {
	(void)request;
    http::Response response(0);
    router.handle(request, response);

    // Print the response status and body
    std::cout
		<< YELLOW "[RESPONSE]" RESET << std::endl
		<< GREEN "Response status code: " RESET << static_cast<int>(response.getStatusCode()) << std::endl
		<< GREEN "Header: " RESET << response.getHeader().toString() << std::endl
		<< GREEN "Response body: \n" RESET << response.getBodyAsString() << std::endl;
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

		// Create a router for each server
		ServerConfig serverConfig = config.servers[2];
		Router router(serverConfig);
		router.get(handleGetRequest);
		//router.post(handlePostRequest);
		//router.del(handleDeleteRequest);

		// Simulate Connection
        int clientSocket = 0; // Dummy socket for testing
        http::Connection connection(clientSocket, [&](http::Request& req, http::Response& res) {
            router.handle(req, res);
        });

        // Simulate reading request
        std::string buffer = "GET /static/about.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
        std::cout << "Raw request header: " << buffer << std::endl;
		connection.readRequest((uint8_t*)buffer.data(), buffer.size());

        // Send response
        connection.sendResponse();

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

