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


volatile sig_atomic_t sigintReceived = 0;

// Function to simulate a request
void simulateRequest(Router& router, http::Request& request) {
	(void)request;
    http::Response response(0);
    router.handle(request, response);

    // Print the response status and body
    std::cout << "Response status: " << static_cast<int>(response.getStatusCode()) << std::endl;
    std::cout << "Response body: \n" << response.getBodyAsString() << std::endl;
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

		std::string rawRequestHeader = "GET /static/../about.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
		std::cout << "Raw request header: " << rawRequestHeader << std::endl;
		http::Request request = http::Request::parseHeader(rawRequestHeader);
		// Print the request method and URL
		utils::printRequest(request);
		simulateRequest(router, request);

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

