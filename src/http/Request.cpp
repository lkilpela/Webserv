#include <sstream>
#include <cstring>
#include <array>
#include <algorithm>
#include <sys/socket.h>
#include "http/Request.hpp"

namespace http {

	const std::string& Request::getMethod() const { return _method; }
	const Uri& Request::getUri() const { return _uri; }
	const std::string& Request::getVersion() const { return _version; }
	// const std::string& Request::getBody() const { return _body; }

	void Request::append(const char* data) {
		if (data != NULL) {
			_buffer.insert(_buffer.end(), data, data + ::strlen(data));
		}
	}

	bool Request::parse() {

	}

	Request& Request::setMethod(const std::string& method) {
		_method = method;
		return *this;
	}

	Request& Request::setUri(const Uri& uri) {
		_uri = uri;
		return *this;
	}

	Request& Request::setVersion(const std::string& version) {
		_version = version;
		return *this;
	}

	Request& Request::setHeader(Header header, const std::string& value) {
		_headers[stringOf(header)] = value;
		return *this;
	}

	// Request& Request::setBody(const std::string& bodyContent) {
	// 	_body = bodyContent;
	// 	return *this;
	// }

	bool Request::isDirectory() const { return _isDirectory; }
	bool Request::isCgi() const { return _isCgi; }

	// std::optional<Request> Request::parse(
	// 	const std::string& rawRequest,
	// 	std::function<bool(const std::string&)> validate
	// ) {
	// 	if (!validate(rawRequest)) {
	// 		return std::nullopt;
	// 	}

	// 	std::istringstream stream(rawRequest);
	// 	std::string line;
	// 	std::string method;
	// 	std::string uri;
	// 	std::string version;
	// 	std::string body;

	// 	std::getline(stream, line);

	// 	std::istringstream requestLineStream(line);

	// 	requestLineStream >> method >> uri >> version;

	// 	Request request;
	// 	request.setMethod(method).setUri(uri).setVersion(version);

	// 	while (std::getline(stream, line) && line != "\r") {
	// 		std::size_t delimiter = line.find(": ");
	// 		std::string key = line.substr(0, delimiter);
	// 		std::string value = line.substr(delimiter + 2);
	// 		request.setHeader(key, value);
	// 	}

	// 	while (std::getline(stream, line) && line != "\r") {
	// 		body.
	// 	}
	// 	return request;
	// }

	// std::variant<Request, HttpError> Request::create(int clientSocket) {
	// 	unsigned char buffer[1024];

	// 	std::memset(buffer, 0, 1024);
	// 	ssize_t bytesRead = ::recv(clientSocket, buffer, 1023, 0);
	// 	if (bytesRead )
	// }

}
