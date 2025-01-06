#include "Config.hpp"
#include "http/Router.hpp"
#include "Error.hpp"

namespace http {
	void Router::get(const Location& config, Handler handler) {
		std::cout << "Router::get request received" << std::endl;

		// Log the config details
		std::cout << YELLOW "Log config details" RESET << std::endl;
        std::cout << "Config path: " << config.path << std::endl;
        std::cout << "Config methods: ";
        for (const auto& method : config.methods) {
            std::cout << method << " ";
        }
        std::cout << std::endl;

		// Check if the GET in the config.methods
		if (std::find(config.methods.begin(), config.methods.end(), "GET") == config.methods.end()) {
			std::cerr << "GET method not allowed for this location" << std::endl;
			throw RouterError(EINVAL, "GET method not allowed for this location");
		}
		// If allowed, add to route 
		_routes[config.path]["GET"] = handler;
		_locationConfigs[config.path] = config;
	}

	void Router::post(const Location& config, Handler handler) {
		std::cout << "Router::post request received" << std::endl;
		if (std::find(config.methods.begin(), config.methods.end(), "POST") == config.methods.end()) {
			throw RouterError(EINVAL, "POST method not allowed for this location");
		}
		_routes[config.path]["POST"] = handler;
		_locationConfigs[config.path] = config;
	}

	void Router::del(const Location& config, Handler handler) {
		std::cout << "Router::del request received" << std::endl;
		if (std::find(config.methods.begin(), config.methods.end(), "DELETE") == config.methods.end()) {
			throw RouterError(EINVAL, "DELETE method not allowed for this location");
		}
		_routes[config.path]["DELETE"] = handler;
		_locationConfigs[config.path] = config;
	}

	const Location* Router::findBestMatchingLocation(const std::string& url) const {
		const Location* bestMatch = nullptr;
		size_t longestMatch = 0;

		for (const auto& [path, config] : _locationConfigs) {
			if (url.substr(0, path.size()) == path && path.length() > longestMatch) {
				bestMatch = &config;
				longestMatch = path.length();
			}
		}
		return bestMatch;
	}

	/**
	 * Handle client request based on the registed `Handler` (callback).
	 * This method call will modify response object in a way defined in callback
	 * 
	 * @param request The request object must be COMPLETE or BAD_REQUEST
	 * @param response
	 */
	void Router::handle(Request& request, Response& response) {
/* 		if (request.getStatus() == Request::Status::BAD_REQUEST) {
			response
				.setStatusCode(StatusCode::BAD_REQUEST_400)
				.setHeader(Header::CONTENT_TYPE, "text/html")
				// root + all the resource files
				.setBody(locations.root + locations.errorPages[400])
				.build();
				.setBody(config.rootPath + "/uploads/abc.text")
				Config::getRootPath() -> absolute path to webserv folder where .
			return;
		} */

		const std::string& route = request.getUrl().path;

		// Find the best matching LocationConfig for the requested route
		const Location* location = findBestMatchingLocation(route);

		ServerConfig serverConfig;

		// No matching location found, return http status 404
		if (!location) {
			std::cout << "Location Not found" << std::endl;
			response
				.setStatusCode(StatusCode::NOT_FOUND_404)
				.setBody(std::make_unique<utils::FilePayload>(0, serverConfig.errorPages[404])) // need to fix this
				.build();
			return;
		}

		// Find the handler for the requested http method
		const auto it = _routes.find(location->path);

		// Matched a route
		if (it != _routes.end()) {
			const auto& handlerByMethod = it->second;
			const auto handlerByMethodIt = handlerByMethod.find(request.getMethod());

			// The matched route does not support the requested http method, return http status 405
			if (handlerByMethodIt == handlerByMethod.end()) {
				std::cout << "Method not allowed" << std::endl;
				response
					.setStatusCode(StatusCode::METHOD_NOT_ALLOWED_405)
					.setBody(nullptr) // need to fix this
					.build();
				return;
			}

			// Execute the handler, return http status 500 if an exception occurs
			try {
				std::cout << "Handler found" << std::endl;
				const auto handler = handlerByMethodIt->second;
				handler(request, response);
			} catch(const std::exception& e) {
				response
					.clear()
					.setStatusCode(StatusCode::INTERNAL_SERVER_ERROR_500)
					.setBody(nullptr) // need to fix this
					.build();
			}

			return;
		}
	}
}
