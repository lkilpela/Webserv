#include <sstream>
#include <fstream>
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