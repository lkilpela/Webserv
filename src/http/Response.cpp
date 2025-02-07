#include <iostream>
#include <sstream>
#include <cstdint>
#include <sys/socket.h>
#include "http/Response.hpp"
#include "Error.hpp"
#include "utils/index.hpp"
#include "http/utils.hpp"

//  Behavior of send()
// When send() returns:
// >= 0: The number of bytes successfully sent.
// < 0: An error occurred. If the error is EAGAIN or EWOULDBLOCK, send() does not block and returns -1.
// Approach
// To detect non-blocking cases (EAGAIN/EWOULDBLOCK):

// If send() returns -1, assume the socket is not ready for writing, and rely on POLLOUT to resume sending.

namespace http {
	Response::Response(int clientSocket)
		: _clientSocket(clientSocket)
		, _header(utils::StringPayload(clientSocket, "")) {
	}

	Response::Response(const Response& other)
		: _clientSocket(other._clientSocket)
		, _status(other._status)
		, _statusCode(other._statusCode)
		, _headerByName(other._headerByName)
		, _header(other._header)
		, _body(other._body ? other._body->clone() : nullptr) {
	}

	Response& Response::operator=(const Response& other) {
		if (this != &other) {
			_clientSocket = other._clientSocket;
			_status = other._status;
			_statusCode = other._statusCode;
			_headerByName = other._headerByName;
			_header = other._header;
			_body = other._body ? other._body->clone() : nullptr;
		}

		return *this;
	}

	bool Response::send() {
		// std::cout << _header.toString();
		// std::cout << _body->toString();

		// if (!_header.isSent()) {
		// 	_header.send();
		// 	return false;
		// }
		// if (_body == nullptr) {
		// 	return true;
		// }

		// _body->send();
		// if (_body->isSent()) {
		// 	return true;
		// }

		// return false;
		// _body->send();
		// _header.send();
		std::string response(
				_header.toString()
			);
		// std::cout << response;
			::send(_clientSocket, response.data(), response.size(), MSG_NOSIGNAL);
		return (true);
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
		std::cout << "ostream:\n" << ostream.str();
		_header.setMessage(ostream.str());
		setStatus(Response::Status::READY);
	}

	int Response::getClientSocket() const {
		return _clientSocket;
	}

	Response::Status Response::getStatus() const {
		return _status;
	}

	StatusCode Response::getStatusCode() const {
		return _statusCode;
	}

	const utils::StringPayload& Response::getHeader() const {
		return _header;
	}

	const std::unique_ptr<utils::Payload>& Response::getBody() const {
		return _body;
	}

	Response& Response::clear() {
		_statusCode = StatusCode::NONE_0;
		_headerByName.clear();
		_header.setMessage("");
		_body.reset();
		return *this;
	}

	Response& Response::setStatus(const Status status) {
		_status = status;
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

	Response& Response::setBody(std::unique_ptr<utils::Payload> body) {
		_body = std::move(body);
		return *this;
	}

	Response& Response::appendBody(const std::uint8_t* data, size_t size) {
		_body->append(data, size);
		return *this;
	}

	void Response::setText(StatusCode statusCode, const std::string& text) {
		setStatusCode(statusCode);
		setBody(std::make_unique<utils::StringPayload>(_clientSocket, text));
		setHeader(Header::CONTENT_TYPE, getMimeType("txt"));
		setHeader(Header::CONTENT_LENGTH, std::to_string(_body->getSizeInBytes()));
		build();
	}

	void Response::setFile(StatusCode statusCode, const std::filesystem::path &filePath) {
		std::string ext = filePath.extension().string().erase(0, 1); // Get file extension without '.'

		setStatusCode(statusCode);
		setBody(std::make_unique<utils::FilePayload>(_clientSocket, filePath));
		setHeader(Header::CONTENT_TYPE, getMimeType(ext));
		setHeader(Header::CONTENT_LENGTH, std::to_string(_body->getSizeInBytes()));
		// setHeader(Header::CACHE_CONTROL, "public, max-age=86400");	// For production mode
		setHeader(Header::CACHE_CONTROL, "no-store"); 				// For test mode
		build();
	}
}
