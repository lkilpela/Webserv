#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include "http/Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
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
<<<<<<< HEAD
=======
    router.addLocations(serverConfig);
    router.get([](Location loc, http::Request& req, http::Response& res) {
        (void)req; // Avoid unused parameter warning
        const std::string& filePath = loc.root + "/" + loc.index;
        std::cout << YELLOW "Serving file: " RESET << filePath << std::endl;
        try {
            // Set the response body to the file contents using FilePayload
            res.setStatusCode(http::StatusCode::OK_200);
            res.setBody(std::make_unique<utils::FilePayload>(0, filePath));
            res.build();
        } catch (const std::ios_base::failure& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            res.setStatusCode(http::StatusCode::NOT_FOUND_404);
            res.setBody(std::make_unique<utils::StringPayload>(0, "File not found"));
            res.build();
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            res.setStatusCode(http::StatusCode::INTERNAL_SERVER_ERROR_500);
            res.setBody(std::make_unique<utils::StringPayload>(0, "Internal Server Error"));
            res.build();
        }
    });
>>>>>>> 27d366f (Refactor Router to support simplified handler registration and improve location management)
}

// Function to simulate a request
void simulateRequest(http::Router& router, const std::string& url) {
    http::Request request;
    request.setMethod("GET");
    request.setUrl(http::parseUri(url.begin(), url.end())); // Make sure this matches the location path
    request.setStatus(http::Request::Status::COMPLETE);

    http::Response response(0);
    router.handle(request, response);

    // Print the response status and body
    std::cout << "Response status: " << static_cast<int>(response.getStatusCode()) << std::endl;
<<<<<<< HEAD
=======
    // Assuming getBody() returns a pointer to a payload object with a toString() method
>>>>>>> 27d366f (Refactor Router to support simplified handler registration and improve location management)
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
		ServerConfig serverConfig = config.servers[0];

        // Debug: Print loaded error pages
        // for (const auto& [code, path] : serverConfig.errorPages) {
        //    std::cout << "Error page " << code << ": " << path << std::endl;
        //}

		//Test the router 
		http::Router router;
		initializeRouter(router, serverConfig);
		simulateRequest(router, "http://localhost:8080/static/");

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
