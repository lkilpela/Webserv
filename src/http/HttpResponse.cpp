#include <sstream>
#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
	: _statusCode(200)
	, _statusText("OK") {

}

HttpResponse::HttpResponse(StatusCode statusCode) {}

unsigned short int HttpResponse::getStatusCode() const { return _statusCode; }
const std::string& HttpResponse::getStatusText() const { return _statusText; }
const std::unordered_map<std::string, std::string>& HttpResponse::getHeaders() const { return _headers; }

void HttpResponse::setStatus(StatusCode statusCode) {

}

void HttpResponse::setHeader(const std::string& name, const std::string& value) {

}

void HttpResponse::setBody(const std::string& bodyContent) {

}

std::string HttpResponse::toString() const {
	std::ostringstream responseStream;

	responseStream << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n";

	for (const auto& [name, value] : _headers) {
		responseStream << name << ": " << value << "\r\n";
	}

	responseStream << "\r\n" << _body;
	return responseStream.str();
}

// int main() {
// 	HttpResponse res;

// 	res.setStatus(HttpResponse::StatusCode::);
// }