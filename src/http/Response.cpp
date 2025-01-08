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
		//: _clientSocket(clientSocket)
		: _header(utils::StringPayload(clientSocket, "")) 
		{}

	bool Response::send() {
		if (!_header.isSent()) {
			_header.send();
			return false;
		}

		if (_body == nullptr) {
			return true;
		}

		_body->send();
		if (_body->isSent()) {
			return true;
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
		_header.setMessage(ostream.str());
	}

	int Response::getClientSocket() const {
		return _clientSocket;
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
