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
/* void simulateRequest(http::Router& router, const http::Url& url) {
    http::Request request;
    request.setMethod("GET");
    request.setUrl(url);
    request.setStatus(http::Request::Status::COMPLETE);

    http::Response response(0);
    router.handle(request, response);

    // Print the response status and body
    std::cout << "Response status: " << static_cast<int>(response.getStatusCode()) << std::endl;
    //std::cout << "Response body: \n" << response.getBody() << std::endl;
}
 */
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
		for (const auto& serverConfig : config.servers) {
			http::Router router;
			router.initRouter(serverConfig, router);
		}

		//http::Url url = http::Url::parse("http://localhost:8080/uploads");
		//simulateRequest(router, url);

		Server server(config);
		//server.listen();
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.what() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
