#include <iostream>
#include "http/Connection.hpp"

int main() {
	std::string rawRequest(
		"GET /static HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\n"
		"Hel\r\n"
		"2\r\n"
		"lo\r\n"
		"6\r\n"
		" World\r\n"
		"0\r\n\r\n"
	);
	using namespace http;
	Connection con(42, [](Request& req, Response& res) {
		std::cout << "Calling cb, body size = " << req.getBody().size() << std::endl;

		for (auto c: req.getBody()) {
			std::cout << c;
		}
		res.setFile(StatusCode::OK_200, "Makefile");
	});

	con.append(rawRequest.data(), rawRequest.size());
	// try {
	// 	auto req = http::Request::parseHeader(rawRequest);
	// 	std::cout
	// 		<< "Status = HEADER_COMPLETE ? " << (req.getStatus() == http::Request::Status::HEADER_COMPLETE) << "\n"
	// 		<< "Method: " << req.getMethod() << "\n"
	// 		<< "Url: " << req.getUrl() << "\n"
	// 		<< "Version: " << req.getVersion() << std::endl;
	// } catch (std::invalid_argument& e) {
	// 	std::cerr << "BAD REQUEST" << std::endl;
	// }

	return 0;
}
