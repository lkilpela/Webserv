#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include "Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Url.hpp"
//#include "Server.hpp"
#include <exception>
#include <thread>
//#include "SignalHandle.hpp"
//#include "http/index.hpp"


//volatile sig_atomic_t sigintReceived = 0;

// Function to handle GET requests
void handleGetRequest(Location loc, http::Request& req, http::Response& res) {
    (void)req; // Avoid unused parameter warning
    const std::string& filePath = loc.root + "/" + loc.index;
    std::cout << YELLOW "Serving file: " RESET << filePath << std::endl;
    try {
        // Set the response body to the file contents using FilePayload
        res.setFileResponse(res, http::StatusCode::OK_200, filePath);
    } catch (const std::ios_base::failure& e) {        
        res.setStringResponse(res, http::StatusCode::NOT_FOUND_404, "File not found");
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        res.setStringResponse(res, http::StatusCode::INTERNAL_SERVER_ERROR_500, "Internal Server Error");
    }
}

// Function to simulate a request
void simulateRequest(http::Router& router, const http::Url& url) {
    http::Request request;
    request.setMethod("GET");
    request.setUrl(url);
    request.setStatus(http::Request::Status::COMPLETE);

    http::Response response(0);
    router.handle(request, response);

    // Print the response status and body
    std::cout << "Response status: " << static_cast<int>(response.getStatusCode()) << std::endl;
    std::cout << "Response body: \n" << response.getBody() << std::endl;
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
		ServerConfig serverConfig = config.servers[1];

		//Test the router 
		http::Router router;
        router.addLocations(serverConfig);

        // Register the GET handler
        router.get(handleGetRequest);
        http::Url url = http::Url::parse("http://localhost:8080/uploads");
		simulateRequest(router, url);

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
