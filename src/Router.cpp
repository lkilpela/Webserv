#include "Config.hpp"
#include "http/Router.hpp"
#include "Error.hpp"

namespace http {

	/* locations path:
	** /
	** /static/
	** /cgi-bin/
	** /uploads/
	*/
	void Router::addLocations(const ServerConfig& serverConfig) {
		_serverConfig = serverConfig;
		for (const auto& location : serverConfig.locations) {
			_locationConfigs[location.path] = location;
		}
	}

	void Router::get(Handler handler) {
		_routes["GET"] = handler;
	}

	void Router::post(Handler handler) {
		_routes["POST"] = handler;
	}

	void Router::del(Handler handler) {
		_routes["DELETE"] = handler;
	}

	const Location* Router::findBestMatchingLocation(const std::string& url) const {
		const Location* bestMatch = nullptr;
		size_t longestMatch = 0;

		std::cout << "Finding best matching location for URL: " << url << std::endl;

		for (const auto& [path, config] : _locationConfigs) {
			if (url.substr(0, path.size()) == path && path.length() > longestMatch) {
				bestMatch = &config;
				longestMatch = path.length();
			}
		}

		if (bestMatch) {
        	std::cout << "Best match found: " << bestMatch->root << std::endl;
		} else {
			std::cout << "No matching location found for URL: " << url << std::endl;
		}
		return bestMatch;
	}

	inline bool isValidPath(const std::string& path) {
		// Check if the path starts and ends with a single slash
		if (path.empty() || path.front() != '/' || path.back() != '/') {
			return false;
		}

		// Check for multiple consecutive slashes
		for (size_t i = 1; i < path.size(); ++i) {
			if (path[i] == '/' && path[i - 1] == '/') {
				return false;
			}
		}

		return true;
	}

	/**
	 * Handle client request based on the registed `Handler` (callback).
	 * This method call will modify response object in a way defined in callback
	 * 
	 * @param request The request object must be COMPLETE or BAD_REQUEST
	 * @param response
	 */
	void Router::handle(Request& request, Response& response) {
		std::string route = request.getUrl().path;

		if (!isValidPath(route)) {
			response.setFileResponse(response, StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
			return;
		}
	/**
	 * Handle client request based on the registed `Handler` (callback).
	 * This method call will modify response object in a way defined in callback
	 * 
	 * @param request The request object must be COMPLETE or BAD_REQUEST
	 * @param response
	 */
	void Router::handle(Request& request, Response& response) {
		std::string route = request.getUrl().path;

		if (!isValidPath(route)) {
			response.setFileResponse(response, StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
			return;
		}

		// Find the best matching LocationConfig for the requested route
		const Location* location = findBestMatchingLocation(route);

		// No matching location found, return HTTP status 404 with the error page
		if (!location) {
			std::cerr << "[ERROR] Location not found: " << route << std::endl;
			response.setFileResponse(response, StatusCode::NOT_FOUND_404, _serverConfig.errorPages[404]);
			return;
		}

		// Find the handler for the requested http method
		const auto it = _routes.find(request.getMethod());

		// Matched a route
		if (it != _routes.end()) {
			// Execute the handler, return http status 500 if an exception occurs
			try {
				const auto handler = it->second;
				handler(*location, request, response);
			} catch(const std::exception& e) {
				response.clear();
				response.setFileResponse(response, StatusCode::INTERNAL_SERVER_ERROR_500, _serverConfig.errorPages[500]);
			}
			return;
		} else {
			// No handler found for the requested method, return http status 405
			response.setFileResponse(response, StatusCode::METHOD_NOT_ALLOWED_405, _serverConfig.errorPages[405]);
			return;
		}
	}
}

