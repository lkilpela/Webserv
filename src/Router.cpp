#include "Config.hpp"
#include "Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "utils/common.hpp"
#include "utils/Payload.hpp"

using http::StatusCode;
using http::Request;
using http::Response;
using std::string;
namespace fs = std::filesystem;

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

	std::cout << "[MATCHING] Finding best matching location for URL: " << url << std::endl;

	for (const auto& [path, config] : _locationConfigs) {
		if (url.substr(0, path.size()) == path && path.length() > longestMatch) {
			bestMatch = &config;
			longestMatch = path.length();
		}
	}

	if (bestMatch) {
		std::cout << "[MATCHING] Best match found: " << bestMatch->root << std::endl;
	} else {
		std::cout << "[MATCHING] No matching location found for URL: " << url << std::endl;
	}
	return bestMatch;
}

std::string getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return filePath.substr(dotPos + 1);
}

// Example: /static/ -> /Users/username/Webserv/config/static/ -> /Users/username/Webserv/config/static/index.html 
std::string generateDirectoryListing(const std::string& path) {
	string listing = "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";

	for (const auto& entry : fs::directory_iterator(path)) {
		const string& name = entry.path().filename().string();
		listing += "<li><a href=\"" + name + "\">" + name + "</a></li>";
	}

	listing += "</ul></body></html>";
	return listing;
}

fs::path computeFilePath(const Location& loc, const Request& req) {
	return loc.root / req.getUrl().path.substr(loc.path.size());
}

void handleDirectoryRequest(const Location& loc, const fs::path& filePath, Response& res) {
	fs::path indexPath = filePath / loc.index;
	if (!fs::exists(indexPath)) {
		indexPath = loc.root / loc.index;
	}

	if (fs::exists(indexPath)) {
		res.setFile(StatusCode::OK_200, indexPath);
	} else if (loc.isAutoIndex) {
		res.setFile(StatusCode::OK_200, generateDirectoryListing(filePath));
	} else {
		res.setFile(StatusCode::FORBIDDEN_403, loc.root / "403.html");
	}
}

// Function to handle GET requests
void handleGetRequest(const Location& loc, Request& req, Response& res) {
	try {
		// Compute the full file path by appending the request subpath
		fs::path filePath = computeFilePath(loc, req);
		std::cout << YELLOW "Location Root: " RESET << loc.root << std::endl;
		std::cout << YELLOW "Request URL Path: " RESET << req.getUrl().path << std::endl;
		std::cout << YELLOW "File Path: " RESET << filePath << std::endl;

		if (fs::is_directory(filePath)) {
			std::cout << YELLOW "Directory request detected" RESET << std::endl;
			handleDirectoryRequest(loc, filePath, res);
		} else if (fs::is_regular_file(filePath)) {
			std::cout << YELLOW "File request detected" RESET << std::endl;
			res.setFile(StatusCode::OK_200, filePath);
		} else {
			std::cout << YELLOW "File not found" RESET << std::endl;
			res.setFile(StatusCode::NOT_FOUND_404, loc.root / "404.html");
		}
	} catch (const std::exception& e) {
		res.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
	}
}

// Function to handle POST requests
void handlePostRequest(const Location& loc, Request& req, Response& res) {
	(void)loc; // Avoid unused parameter warning
	(void)req; // Avoid unused parameter warning
	res.setFile(http::StatusCode::OK_200, "POST request received");
}

// Function to handle DELETE requests
void handleDeleteRequest(const Location& loc, Request& req, Response& res) {
	(void)loc; // Avoid unused parameter warning
	(void)req; // Avoid unused parameter warning
	res.setFile(http::StatusCode::OK_200, "DELETE request received");
}

void logRequestStatus(const Request& request) {
	std::cout << YELLOW "Request status after calling handler " RESET << std::endl;
	if (request.getStatus() == Request::Status::INCOMPLETE) {
		std::cout << GREEN "Request status: " RESET << "INCOMPLETE" << std::endl;
	} else if (request.getStatus() == Request::Status::HEADER_COMPLETE) {
		std::cout << GREEN "Request status: " RESET << "HEADER_COMPLETE" << std::endl;
	} else if (request.getStatus() == Request::Status::BAD) {
		std::cout << GREEN "Request status: " RESET << "BAD" << std::endl;
	} else if (request.getStatus() == Request::Status::COMPLETE) {
		std::cout << GREEN "Request status: " RESET << "COMPLETE" << std::endl;
	}
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
	if (request.getStatus() == Request::Status::BAD) {
		response.setFile(StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
		return;
	}

	std::string requestPath = request.getUrl().path;
	std::cout << "\n[HANDLE()] Handling request for path: " << requestPath << std::endl;

/* 	if (!utils::isValidPath(requestPath)) {
		std::cerr << "[ERROR] Invalid path: " << requestPath << std::endl;
		setFileResponse(response, StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
		return;
	} */

	// Find the best matching LocationConfig for the requested route
	// example: location = /static/
	const Location* location = findBestMatchingLocation(requestPath);
	// No matching location found, return HTTP status 404 with the error page
	if (!location) {
		std::cerr << "[ERROR] Location not found: " << requestPath << std::endl;
		response.setFile(StatusCode::NOT_FOUND_404, _serverConfig.errorPages[404]);
		return;
	}

	// Find the handler for the requested http method
	const auto it = _routes.find(request.getMethod());

	// Matched a route
	if (it != _routes.end()) {
		// Execute the handler, return http status 500 if an exception occurs
		try {
			std::cout << YELLOW "Calling handler for method: " RESET << request.getMethod() << std::endl;
			// example: handler = handleGetRequest
			// it->second = handleGetRequest
			// it->first = GET
			const auto handler = it->second;
			handler(*location, request, response);
			// Set request status to COMPLETE if handler succeeds
			request.setStatus(Request::Status::COMPLETE);
			logRequestStatus(request);
		} catch(const std::exception& e) {
			response.clear();
			response.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, _serverConfig.errorPages[500]);
		}
		return;
	} else {
		// No handler found for the requested method, return http status 405
		response.setFile(StatusCode::METHOD_NOT_ALLOWED_405, _serverConfig.errorPages[405]);
		return;
	}
}
