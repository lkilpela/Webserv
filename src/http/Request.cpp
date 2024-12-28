#include <sstream>
#include <cstring>
#include <array>
#include <algorithm>
#include <sys/socket.h>
#include "utils.hpp"
#include "http/Request.hpp"

// Behavior of recv()
// When recv() returns:
// > 0: Data was successfully received.
// 0: The peer has performed an orderly shutdown (connection closed).
// < 0: An error occurred. If the error is EAGAIN or EWOULDBLOCK, recv() does not block and returns -1 immediately.
// Approach
// To detect non-blocking cases (EAGAIN/EWOULDBLOCK):

// If recv() returns -1 and the socket is non-blocking, assume it’s a temporary error and retry when POLLIN is triggered again.

namespace http {

	Request::Request(Status status) : _status(status) {}

	void Request::clear() {
		_method.clear();
		_url = Url();
		_version.clear();
		_headers.clear();
		_body.clear();
		_status = Request::Status::INCOMPLETE;
	}

	const std::string& Request::getMethod() const {
		return _method;
	}

	const Url& Request::getUrl() const {
		return _url;
	}

	const std::string& Request::getVersion() const {
		return _version;
	}

	std::optional<std::string> Request::getHeader(Header header) const {
		auto it = _headers.find(stringOf(header));

		if (it == _headers.end()) {
			return std::nullopt;
		}

		return it->second;
	}

	const std::span<const std::uint8_t> Request::getBody() const {
		return std::span<const std::uint8_t>(_body);
	}

	Request::Status Request::getStatus() const {
		return _status;
	}

	Request& Request::setMethod(const std::string& method) {
		_method = method;
		return *this;
	}

	Request& Request::setUrl(const Url& url) {
		_url = url;
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

	Request& Request::setStatus(Request::Status status) {
		_status = status;
		return *this;
	}

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
