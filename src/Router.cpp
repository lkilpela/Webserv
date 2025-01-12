#include "Config.hpp"
#include "Router.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "utils/common.hpp"
#include "utils/Payload.hpp"
#include "http/Url.hpp"

using http::StatusCode;
using http::Request;
using http::Response;
using std::string;
namespace fs = std::filesystem;

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
		std::cout << "[MATCHING] Checking location: " << path << std::endl;
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

fs::path computeFilePath(const Location& loc, const string& requestPath) {
	return loc.root / requestPath.substr(loc.path.size());
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

void handleRedirectRequest(const Location& loc, Response& res, Request& req) {
	std::string redirectUrl = loc.returnUrl[1]; // Extract redirect URL
	res.setHeader(http::Header::LOCATION, redirectUrl);
	res.setStatusCode(http::StatusCode::MOVED_PERMANENTLY_301);
	res.build();
	req.setStatus(Request::Status::COMPLETE);
	logRequestStatus(req);
}

// Function to handle GET requests
void handleGetRequest(const Location& loc, const string& requestPath, Request& req, Response& res) {
	(void) req;
	try {
		// Compute the full file path by appending the request subpath
		fs::path filePath = computeFilePath(loc, requestPath);
		std::cout << YELLOW "Location Root: " RESET << loc.root << std::endl;
		std::cout << YELLOW "Request URL Path: " RESET << requestPath << std::endl;
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
		res.setString(StatusCode::INTERNAL_SERVER_ERROR_500, "Internal Server Error");
	}
}

// Function to handle POST requests
void handlePostRequest(const Location& loc, const string& requestPath, Request& req, Response& res) {
	// Extract data from the request body
	// Check if uploads are allowed
	// Save the file to the upload directory
	// Return 200 OK if the file is saved successfully
	// Return 403 Forbidden if uploads are not allowed
	// Return 500 Internal Server Error if an exception occurs
	// Return 400 Bad Request if the file cannot be saved
	// Example: /uploads/ -> /Users/username/Webserv/config/uploads/ -> /Users/username/Webserv/config/uploads/index.html
	try {
		std::cout << GREEN "In handlePostRequest()" RESET << std::endl;
		fs::path uploadPath = computeFilePath(loc, requestPath);
		std::cout << YELLOW "Upload Path: " RESET << uploadPath << std::endl;

		std::ofstream file(uploadPath.string(), std::ios::binary);
		if (!file) {
			std::cerr<< YELLOW "Failed to open file" RESET << std::endl;
			res.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
			return;
		}
		file.write(reinterpret_cast<const char*>(req.getBody().data()), req.getBody().size());
		res.setString(http::StatusCode::OK_200, "File uploaded successfully");
	} catch (const std::exception& e) {
		std::cerr << YELLOW "Exception: " RESET << e.what() << std::endl;
		res.setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
	}
}

// Function to handle DELETE requests
void handleDeleteRequest(const Location& loc, const string& requestPath, Request& req, Response& res) {
	(void) req;
	try {
		fs::path filePath = computeFilePath(loc, requestPath);
		std::cout << YELLOW "Delete file path: " RESET << filePath << std::endl;
		if (!fs::exists(filePath)) {
			res.setFile(http::StatusCode::NOT_FOUND_404, loc.root / "404.html");
			return;
		}

		if (fs::remove(filePath)) {
			res.setString(http::StatusCode::OK_200, "File deleted successfully");
			return;
		} else {
			res.setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
			return;
		}
	} catch (const std::exception& e) {
		res.setFile(http::StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
	}
}

// Hander function to handle requests based on the method and matching location
void Router::handle(Request& request, Response& response) {
	if (request.getStatus() == Request::Status::BAD) {
		response.setFile(StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
		return;
	}

	requestPath = request.getUrl().path;
	std::cout << "\n[HANDLE()] Handling request for path: " << requestPath << std::endl;

	requestPath = utils::lowerCase(requestPath);
	
	if (!requestPath.empty() && !requestPath.ends_with('/')) {
		requestPath = requestPath + "/";
	}

	if (!utils::isValidPath(requestPath)) {
		std::cerr << "[ERROR] Invalid path: " << requestPath << std::endl;
		response.setFile(StatusCode::BAD_REQUEST_400, _serverConfig.errorPages[400]);
		return;
	}

	const Location* location = findBestMatchingLocation(requestPath);
	if (!location) {
		std::cerr << "[ERROR] Location not found: " << requestPath << std::endl;
		response.setFile(StatusCode::NOT_FOUND_404, _serverConfig.errorPages[404]);
		return;
	}

	// Check for redirect
	if (!location->returnUrl.empty()) {
		std::cout << YELLOW "Redirecting to: " RESET << location->returnUrl[1] << std::endl;
		handleRedirectRequest(*location, response, request);
		return;
	}

	// Find the handler for the requested http method
	const auto it = _routes.find(request.getMethod());

	// Matched a route
	if (it != _routes.end()) {
		try {
			std::cout << YELLOW "Calling handler for method: " RESET << request.getMethod() << std::endl;
			const auto handler = it->second;
			handler(*location, requestPath, request, response);
			request.setStatus(Request::Status::COMPLETE);
			logRequestStatus(request);
		} catch(const std::exception& e) {
			response.clear();
			response.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, _serverConfig.errorPages[500]);
		}
		return;
	} else {
		response.setFile(StatusCode::METHOD_NOT_ALLOWED_405, _serverConfig.errorPages[405]);
		return;
	}
}
