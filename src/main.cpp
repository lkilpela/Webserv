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

// Function to initialize the router and set up the GET handler
void initializeRouter(http::Router& router, const ServerConfig& serverConfig) {
    if (serverConfig.locations.size() == 0) {
        throw ConfigError(EINVAL, "No locations found in the configuration file");
    }
    router.get(serverConfig.locations[0], [&serverConfig](http::Request& req, http::Response& res) {
        std::cout << "GET request received" << std::endl;
        (void)req; // Avoid unused parameter warning

        const std::string& filePath = serverConfig.locations[0].root + "/index.html";
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
    // Assuming getBody() returns a pointer to a payload object with a toString() method
    std::cout << "Response body: " << response.getBody() << std::endl;
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

		//Test the router 
		http::Router router;
		initializeRouter(router, serverConfig);
		simulateRequest(router, "http://localhost:8080/");

		//Server server(config);
		//server.listen();
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.code().message() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
