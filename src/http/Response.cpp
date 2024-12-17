#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "http/Response.hpp"

//  Behavior of send()
// When send() returns:
// >= 0: The number of bytes successfully sent.
// < 0: An error occurred. If the error is EAGAIN or EWOULDBLOCK, send() does not block and returns -1.
// Approach
// To detect non-blocking cases (EAGAIN/EWOULDBLOCK):

// If send() returns -1, assume the socket is not ready for writing, and rely on POLLOUT to resume sending.

namespace http {

	// Response::Response(const Response& response)
	// 	: _statusCode(response._statusCode)
	// 	, _statusText(response._statusText)
	// 	, _type(response._type)
	// 	, _url(response._url)
	// 	, _body(response._body)
	// 	, _headers(response._headers) {
	// }

	// Response& Response::operator=(const Response& response) {
	// 	if (this != &response) {
	// 		_statusCode = response._statusCode;
	// 		_statusText = response._statusText;
	// 		_type = response._type;
	// 		_url = response._url;
	// 		_body = response._body;
	// 		_headers = response._headers;
	// 	}
	// 	return *this;
	// }

	const Status& Response::getStatus() const { return _status; }

	Response& Response::setStatus(const Status& status) {
		_status = status;
		return *this;
	}

	Response& Response::setHeader(Header header, const std::string& value) {
		_headers[stringOf(header)] = value;
		return *this;
	}

	Response& Response::setBody(const std::string& bodyContent) {
		_body = bodyContent;
		return *this;
	}

	std::string Response::_composeHeaders() const {
		std::ostringstream stream;

		stream
			<< "HTTP/1.1 "
			<< static_cast<std::uint16_t>(_status.code) << " "
			<< _status.reason << "\r\n";

		for (const auto& [name, value] : _headers) {
			stream << name << ": " << value << "\r\n";
		}

		stream << "\r\n";
		return stream.str();
	}

	void Response::_send(int clientSocket, const void *buf, const size_t size, int flags) {
		const ssize_t bytesSent = ::send(clientSocket, buf + _totalBytesSent, size - _totalBytesSent, flags);
		if (bytesSent < 0) {
			// throw SocketException("Failed to send data");
		}
		_totalBytesSent += static_cast<std::size_t>(bytesSent);
		if (_totalBytesSent >= size) {
			_isComplete = true;
		}
	}

	void Response::sendFile(const std::string& filePath) {
		std::ifstream file(filePath);


	}

	void Response::sendStatus(int clientSocket, Status status) {
		setStatus(status);
		setHeader(Header::CONTENT_TYPE, "text/plain");
		setHeader(Header::CONTENT_LENGTH, std::to_string(_status.reason.length()));

		std::string response = _composeHeaders() + _status.reason;
		_send(clientSocket, response.c_str(), response.size(), 0);
	}

	void Response::sendText(const std::string& text) {

	}

	bool Response::isComplete() const {
		return _isComplete;
	}

}

