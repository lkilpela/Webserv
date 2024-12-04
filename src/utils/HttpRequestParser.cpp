#include <iostream>
#include <sstream>
#include "utils/HttpRequestParser.hpp"

namespace utils {

	std::unordered_map<std::string, std::string> HttpRequestParser::parseHeaders(std::string& rawHeader) {
		std::istringstream inputStream(rawHeader);
		std::unordered_map<std::string, std::string> map;

		std::string line;
		while (std::getline(inputStream, line) && line != "\r")
		{
			std::size_t delimiter = line.find(": ");
			if (delimiter == std::string::npos) {
				std::cout << line << std::endl;
				throw std::runtime_error("Malformed header field: " + line);
			}
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 2);
			map[key] = value;
		}
		return map;
	}

}

int main() {
	std::string rawHeader = "Host: www.example.re\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\nAccept: */*\r\nAccept-Language: en-US, en; q=0.5\r\nAccept-Encoding: gzip, deflate\r\n\r\n";
	auto map = utils::HttpRequestParser::parseHeaders(rawHeader);
	for (const auto& [key, value] : map) {
		std::cout << key << ": " << value << std::endl;
	}
}
