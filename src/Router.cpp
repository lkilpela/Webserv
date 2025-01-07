#include "Config.hpp"
#include "Router.hpp"

using http::StatusCode;
using http::Request;
using http::Response;
using std::string;

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

const Location* Router::findBestMatchingLocation(const string& url) const {
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

inline bool isValidPath(const string& path) {
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

// Function to set the response for string payloads
void setStringResponse(Response& res, StatusCode statusCode, const string& body) {
	res.setStatusCode(statusCode);
	res.setBody(std::make_unique<utils::StringPayload>(0, body));
	res.build();
}

// Function to set the response for file payloads
void setFileResponse(Response& res, StatusCode statusCode, const string& filePath) {
	res.setStatusCode(statusCode);
	res.setBody(std::make_unique<utils::FilePayload>(0, filePath));
	res.build();
}

// Function to handle GET requests
void handleGetRequest(const Location& loc, Request& req, Response& res) {
	// Create a response body with the file contents
	(void)req; // Avoid unused parameter warning
	const string& filePath = loc.root + "/" + loc.index;
	std::cout << YELLOW "Serving file: " RESET << filePath << std::endl;
	//try {
		// Set the response body to the file contents using FilePayload
		setFileResponse(res, StatusCode::OK_200, filePath);
/* 		} catch (const std::ios_base::failure& e) {        
		res.setStringResponse(res, http::StatusCode::NOT_FOUND_404, "File not found");
	} catch (const std::exception& e) {
		std::cerr << "Unexpected error: " << e.what() << std::endl;
		res.setStringResponse(res, http::StatusCode::INTERNAL_SERVER_ERROR_500, "Internal Server Error");
	} */
}

// Function to handle POST requests
void handlePostRequest(const Location& loc, Request& req, Response& res) {
	(void)loc; // Avoid unused parameter warning
	(void)req; // Avoid unused parameter warning
	setStringResponse(res, http::StatusCode::OK_200, "POST request received");
}

// Function to handle DELETE requests
void handleDeleteRequest(const Location& loc, Request& req, Response& res) {
	(void)loc; // Avoid unused parameter warning
	(void)req; // Avoid unused parameter warning
	setStringResponse(res, http::StatusCode::OK_200, "DELETE request received");
}

/**
 * Handle client request based on the registed `Handler` (callback).
 * This method call will modify response object in a way defined in callback
 * 
 * @param request The request object must be COMPLETE or BAD_REQUEST
 * @param response
 */

// Connection calls router.handle(request, response)
// Router calls handler(*location, request, response)
void Router::handle(Request& request, Response& response) {
	// example: route = /static/
	std::string requestPath = request.getUrl().path;
	std::cout << "Handling request for path: " << requestPath << std::endl;

	if (!isValidPath(requestPath)) {
		setFileResponse(response, StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
		return;
	}

	// Find the best matching LocationConfig for the requested route
	// example: location = /static/
	const Location* location = findBestMatchingLocation(requestPath);

	// No matching location found, return HTTP status 404 with the error page
	if (!location) {
		std::cerr << "[ERROR] Location not found: " << requestPath << std::endl;
		setFileResponse(response, StatusCode::NOT_FOUND_404, _serverConfig.errorPages[404]);
		return;
	}

	// Find the handler for the requested http method
	const auto it = _routes.find(request.getMethod());

	// Matched a route
	if (it != _routes.end()) {
		// Execute the handler, return http status 500 if an exception occurs
		try {
			// example: handler = handleGetRequest
			// it->second = handleGetRequest
			// it->first = GET
			const auto handler = it->second;
			handler(*location, request, response);
		} catch(const std::exception& e) {
			response.clear();
			setFileResponse(response, StatusCode::INTERNAL_SERVER_ERROR_500, _serverConfig.errorPages[500]);
		}
		return;
	} else {
		// No handler found for the requested method, return http status 405
		setFileResponse(response, StatusCode::METHOD_NOT_ALLOWED_405, _serverConfig.errorPages[405]);
		return;
	}
}

