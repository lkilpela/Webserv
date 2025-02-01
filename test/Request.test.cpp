#include <iostream>
#include "http/index.hpp"

int main() {
	std::string rawRequest(
		"GET http://example.com/old-docs HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"
	);

	try {
		auto req = http::Request::parseHeader(rawRequest);
		std::cout
			<< "Status = HEADER_COMPLETE ? " << (req.getStatus() == http::Request::Status::HEADER_COMPLETE) << "\n"
			<< "Method: " << req.getMethod() << "\n"
			<< "Url: " << req.getUrl() << "\n"
			<< "Version: " << req.getVersion() << std::endl;
	} catch (std::invalid_argument& e) {
		std::cerr << "BAD REQUEST" << std::endl;
	}

	return 0;
}
