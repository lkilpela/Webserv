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
		// Create a routerjj    
		http::Router router;
		//Testing router.get
		if (serverConfig.locations.size() == 0) {
			throw ConfigError(EINVAL, "No locations found in the configuration file");
		}
		router.get(serverConfig.locations[0], [](http::Request& req, http::Response& res) {
			std::cout << "GET request received" << std::endl;
			(void)req; // Avoid unused parameter warning
			res.setStatusCode(http::StatusCode::OK_200);
			res.setBody(std::make_unique<utils::StringPayload>(0, "Hello, World!"));
		});

      // Simulate a request
        http::Request request;
        request.setMethod("GET");
		std::string s = "/";
        request.setUrl(http::parseUri(s.begin(), s.end())); // Make sure this matches the location path
        request.setStatus(http::Request::Status::COMPLETE);

        http::Response response(0);
        router.handle(request, response);

 		// Print the response status and body
        std::cout << "Response status: " << static_cast<int>(response.getStatusCode()) << std::endl;
        // Assuming getBody() returns a pointer to a payload object with a toString() method
        //std::cout << "Response body: " << response.setBody() << std::endl;

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
