#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include "Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
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
    //std::cout << "Response body: \n" << response.getBody() << std::endl;
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
		ServerConfig serverConfig = config.servers[0];
		Router router(serverConfig);
		router.get(handleGetRequest);

		std::string rawRequestHeader = "GET /static/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
		std::cout << "Raw request header: " << rawRequestHeader << std::endl;
		http::Request request = http::Request::parseHeader(rawRequestHeader);
		// Print the request method and URL
		std::cout << "Request method: " << request.getMethod() << std::endl;
		std::cout << "Request URL_scheme: " << request.getUrl().scheme << std::endl;
		std::cout << "Request URL_user: " << request.getUrl().user << std::endl;
		std::cout << "Request URL_password: " << request.getUrl().password<< std::endl;
		std::cout << "Request URL_host: " << request.getUrl().host << std::endl;
		std::cout << "Request URL_port: " << request.getUrl().port << std::endl;
		std::cout << "Request URL_path: " << request.getUrl().path << std::endl;
		std::cout << "Request URL_query: " << request.getUrl().query << std::endl;
		std::cout << "Request URL_fragment: " << request.getUrl().fragment << std::endl;
		std::cout << "Request version: " << request.getVersion() << std::endl;
		std::cout << "Request body size: " << request.getBodySize() << std::endl;
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
