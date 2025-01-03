#include <iostream>
#include "http/index.hpp"

int main() {
	std::string rawRequest(
		"GET / HTTP/1.1\r\nHost: example.com\r\n\r\n"
	);

	auto req = http::Request::parseHeader(rawRequest);
	std::cout << req.getMethod() << req.getUrl().path << req.getVersion() << std::endl;

	return 0;
}