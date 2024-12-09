#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "http/Response.hpp"

using http::Response;
using http::Status;
// using Map = std::unordered_map<std::string, std::string>;

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

const Status& Response::getStatus() const { return _status; }

const std::unordered_map<std::string, std::string>& Response::getHeaders() const { return _headers; }

Response& Response::setStatus(const Status& status) {
	_status = status;
	return *this;
}

Response& Response::setHeader(const std::string& name, const std::string& value) {
	return *this;
}

Response& Response::setBody(const std::string& bodyContent) {
	_body = bodyContent;
	return *this;
}

// void Response::sendFile(const std::string& filePath, std::function<void(std::exception&)> onError) {
// 	std::ifstream file(filePath);

// 	if (!file.is_open()) {
// 		onError()
// 	}
// }
void Response::sendStatus(Status status) {
	std::ostringstream stream;

	stream
		<< "HTTP/1.1 "
		<< static_cast<std::uint16_t>(_status.code) << " "
		<< _status.reason << "\r\n"
		<< "Content-Type: text/plain\r\n"
		<< "Content-Length: " << _status.reason.length() << "\r\n\r\n"
		<< _status.reason;


	// for (const auto& [name, value] : _headers) {
	// 	stream << name << ": " << value << "\r\n";
	// }

	std::string data = stream.str();
	size_t totalBytes = data.size();
	size_t totalBytesSent = 0;

	while (totalBytesSent < totalBytes) {
		ssize_t bytesSent = ::send(_clientSocket, data.c_str(), totalBytes - bytesSent, 0);
		if (bytesSent < 0) {
			std::cerr << "Failed to send data at Response::sendStatus(): " << ::strerror(errno) << std::endl;
			break;
		}
		totalBytesSent += static_cast<size_t>(bytesSent);
	}
	if (::close(_clientSocket) < 0) {
		std::cerr << "Failed to close socket at Response::sendStatus(): " << ::strerror(errno) << std::endl;
	}
}
// std::string Response::toString() const {
// 	std::ostringstream responseStream;

// 	responseStream << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n";

// 	for (const auto& [name, value] : _headers) {
// 		responseStream << name << ": " << value << "\r\n";
// 	}

// 	responseStream << "\r\n" << _body;
// 	return responseStream.str();
// }

// int main() {
// 	Response res;

// 	res.setStatus(Response::StatusCode::);
// }
