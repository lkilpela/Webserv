#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include "http/Response.hpp"
#include "utils.hpp"

//  Behavior of send()
// When send() returns:
// >= 0: The number of bytes successfully sent.
// < 0: An error occurred. If the error is EAGAIN or EWOULDBLOCK, send() does not block and returns -1.
// Approach
// To detect non-blocking cases (EAGAIN/EWOULDBLOCK):

// If send() returns -1, assume the socket is not ready for writing, and rely on POLLOUT to resume sending.

namespace http {
	Response::Response(int clientSocket) : _clientSocket(clientSocket) {}

	bool Response::send() {
		if (!_sendHeader())
			return false;

		if (_body == nullptr)
			return true;

		if (_body->send())
			return true;

		return false;
	}

	void Response::build() {
		std::ostringstream ostream;

		ostream
			<< "HTTP/1.1 "
			<< static_cast<std::uint16_t>(_statusCode) << " "
			<< stringOf(_statusCode) << "\r\n";

		for (const auto& [name, value] : _headers) {
			ostream << name << ": " << value << "\r\n";
		}

		ostream << "\r\n";
		_header = ostream.str();
	}

	const Response::Status& Response::getStatus() const {
		return _status;
	}

	const http::StatusCode Response::getHttpStatusCode() const {
		return _statusCode;
	}

	Response& Response::setHttpStatusCode(const http::StatusCode statusCode) {
		_statusCode = statusCode;
		return *this;
	}

	Response& Response::setHeader(Header header, const std::string& value) {
		_headers[stringOf(header)] = value;
		return *this;
	}

	Response& Response::setBody(std::unique_ptr<ResponseBody> body) {
		_body = std::move(body);
		return *this;
	}

	bool Response::_sendHeader() {
		const ssize_t bytesSent = ::send(
			_clientSocket,
			_header.data() + this->_bytesSent,
			_header.size() - this->_bytesSent,
			MSG_NOSIGNAL
		);

		if (bytesSent >= 0) {
			this->_bytesSent += static_cast<std::size_t>(bytesSent);

			if (this->_bytesSent >= _header.size())
				return true;
		}

		return false;
	}
}
