#include <iostream>
#include "http/index.hpp"

int main() {
	std::string rawRequest(
		"GET /static/image.jpg HTTP/1.1\r\nHost: example.com\r\n\r\n"
	);

	auto req = http::Request::parseHeader(rawRequest);
	std::cout
		<< "Status = HEADER_COMPLETE ? " << (req.getStatus() == http::Request::Status::HEADER_COMPLETE) << "\n"
		<< "Method: " << req.getMethod() << "\n"
		<< "Url: " << req.getUrl() << "\n"
		<< "Version: " << req.getVersion() << std::endl;

	// int fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	return 0;
}