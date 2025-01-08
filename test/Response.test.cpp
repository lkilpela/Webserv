#include <iostream>
#include "http/Response.hpp"

int main() {
	http::Response res(42);
	res.setFile(http::StatusCode::OK_200, "README.md");

	std::cout
		<< res.getHeader().toString() << "\n"
		<< res.getBody()->toString() << std::endl;
	return 0;
}