#include <sstream>
#include <cstdint>
#include <sys/socket.h>
#include "http/Response.hpp"
#include "Error.hpp"
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
		if (this->_bytesSent >= _header.size()) {
			return true;
		}

		const ssize_t bytesSent = ::send(
			_clientSocket,
			_header.data() + this->_bytesSent,
			_header.size() - this->_bytesSent,
			MSG_NOSIGNAL
		);

		if (bytesSent >= 0) {
			this->_bytesSent += static_cast<std::size_t>(bytesSent);

			if (this->_bytesSent >= _header.size()) {
				return true;
			}
		}

		return false;
	}

	void Response::build() {
		std::ostringstream ostream;

		ostream
			<< "HTTP/1.1 "
			<< static_cast<std::uint16_t>(_statusCode) << " "
			<< stringOf(_statusCode) << "\r\n";

		for (const auto& [name, value] : _headerByName) {
			ostream << name << ": " << value << "\r\n";
		}

		ostream << "\r\n";
		_header = ostream.str();
	}

	const StatusCode Response::getStatusCode() const {
		return _statusCode;
	}

	Response& Response::clear() {
		_statusCode = StatusCode::NONE_0;
		_headerByName.clear();
		_header.clear();
		_bytesSent = 0;
		return *this;
	}

	Response& Response::setStatusCode(const StatusCode statusCode) {
		_statusCode = statusCode;
		return *this;
	}

	Response& Response::setHeader(Header header, const std::string& value) {
		_headerByName[stringOf(header)] = value;
		return *this;
	}
}
