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

	Response::Response(int clientSocket) : _clientSocket(clientSocket) {}

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
	ssize_t Response::_send() {
		if (_status != Response::Status::READY) {
			return 0;
		}
		_status = Response::Status::SENDING;
		const ssize_t bytesSent = ::send(
			_clientSocket,
			buf + this->_bytesSent,
			_totalBytes - this->_bytesSent,
			MSG_NOSIGNAL
		);
		if (bytesSent < 0) {
			// throw SocketException("Failed to send data");
		}
		this->_bytesSent += static_cast<std::size_t>(bytesSent);
		if (_totalBytesSent >= size) {
			_isComplete = true;
		}
		return bytesSent;
	}

	const http::Status& Response::getHttpStatus() const {
		return _httpStatus;
	}

	Response& Response::setHttpStatus(const http::Status& httpStatus) {
		_httpStatus = httpStatus;
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

	void Response::sendFile(const std::string& filePath) {
		
		std::ifstream file(filePath);


	}

	// void Response::sendStatus(int clientSocket, Status status) {
	// 	setStatus(status);
	// 	setHeader(Header::CONTENT_TYPE, "text/plain");
	// 	setHeader(Header::CONTENT_LENGTH, std::to_string(_status.reason.length()));

	// 	std::string response = _composeHeaders() + _status.reason;
	// 	_send(clientSocket, response.c_str(), response.size(), 0);
	// }

	// void Response::sendText(const std::string& text) {

	// }

}

