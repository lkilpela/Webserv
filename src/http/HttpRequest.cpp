#include <sstream>
#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const std::string& rawRequest) {
	std::istringstream stream(rawRequest);
	std::string line;

	std::getline(stream, line);
	std::istringstream requestLine(line);
	requestLine >> _method >> _uri >> _version;

	while (std::getline(stream, line) && line != "\r")
	{
		/* code */
	}

}
// HttpRequest::HttpRequest(const HttpRequest& request);
// HttpRequest::HttpRequest& operator=(const HttpRequest& request);

const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getUri() const { return _uri; }
const std::string& HttpRequest::getVersion() const { return _version; }
const std::unordered_map<std::string, std::string>& HttpRequest::getHeaders() const { return _headers; }
const std::string& HttpRequest::getBody() const { return _body; }

bool HttpRequest::isDirectory() const { return _isDirectory; }
bool HttpRequest::isCgi() const { return _isCgi; }
