#include <sstream>
#include "HttpRequest.hpp"

// HttpRequest::HttpRequest(const HttpRequest& request);
// HttpRequest::HttpRequest& operator=(const HttpRequest& request);

const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getUri() const { return _uri; }
const std::string& HttpRequest::getVersion() const { return _version; }
const std::unordered_map<std::string, std::string>& HttpRequest::getHeaders() const { return _headers; }
const std::string& HttpRequest::getBody() const { return _body; }

void HttpRequest::setHeader(const std::string& name, const std::string& value) {
	_headers[name] = value;
}

bool HttpRequest::isDirectory() const { return _isDirectory; }
bool HttpRequest::isCgi() const { return _isCgi; }

std::variant<HttpRequest, ParseError> HttpRequest::parse(const std::string& rawRequest) {
	HttpRequest request;
	std::istringstream inputStream(rawRequest);

	std::string line;
	while (std::getline(inputStream, line) && line != "\r")
	{
		std::size_t delimiter = line.find(": ");
		if (delimiter == std::string::npos) {
			throw std::runtime_error("Malformed header field: " + line);
		}
		std::string key = line.substr(0, delimiter);
		std::string value = line.substr(delimiter + 2);
		request.setHeader(key, value);
	}
	return request;
}
