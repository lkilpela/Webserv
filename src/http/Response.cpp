#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
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

	void Response::send() {
		using enum Response::Status;

		if (_status != READY || _status != SENDING) return 0;
		_status = SENDING;

		if (!_header.empty()) {
			const ssize_t bytesSent = ::send(_clientSocket, _header.c_str(), _header.size(), MSG_NOSIGNAL);
			if (bytesSent < 0) {
				// throw SocketException("Failed to send data");
			}
		} else {
			_sendBody();
		}
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

	Response& Response::setBody(const std::string& bodyContent) {
		_body = bodyContent;
		return *this;
	}

	Response& Response::setFile(const std::string& filePath) {
		_filePath = filePath;
		return *this;
	}

	void Response::build() {
		_header = buildResponseHeader(_statusCode, _headers);

		if (!_filePath.empty()) {
			_fileStream.open(_filePath);
			if (!_fileStream.is_open()) {
				std::cerr << "Failed to open file " << _filePath << std::endl;
				_status = Response::Status::ERROR;
				throw ;
			}
		}
		_status = Response::Status::READY;
	}

	void Response::_sendBody() {
		const ssize_t bytesSent = ::send(_clientSocket, _body.data(), _body.size(), MSG_NOSIGNAL);
		if (bytesSent >= 0) {
			_body.erase(0, static_cast<std::size_t>(bytesSent));
			if (_body.empty()) {
				_status = Response::Status::SENT_ALL;
			}
		}

		std::array<char, 1024> buffer;
		const size_t bytesRead = _fileStream
									.read(buffer.data(), buffer.size())
									.gcount();
		const ssize_t bytesSent = ::send(_clientSocket, buffer.data(), bytesRead, MSG_NOSIGNAL);
		if (bytesSent >= 0) {

		}
	}
}

