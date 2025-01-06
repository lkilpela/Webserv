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

	// Function to get the body of the response
	std::string getBody() const {
		if (_body) {
			return _body->toString();
		}
		return "";
	} 

	// Function to set the response for string payloads
	void Response::setStringResponse(Response& res, StatusCode statusCode, const std::string& body) {
		res.setStatusCode(statusCode);
		res.setBody(std::make_unique<utils::StringPayload>(0, body));
		res.build();
	}

	// Function to set the response for file payloads
	void Response::setFileResponse(Response& res, StatusCode statusCode, const std::string& filePath) {
		res.setStatusCode(statusCode);
		res.setBody(std::make_unique<utils::FilePayload>(0, filePath));
		res.build();
	}
}
