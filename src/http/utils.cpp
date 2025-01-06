#include <sstream>
#include <regex>
#include "utils/common.hpp"
#include "http/utils.hpp"

namespace http {
	bool hasHeaderName(const std::string &headerName) {
		for (int i = 0; i < static_cast<int>(Header::LENGTH); i++) {
			Header header = static_cast<Header>(i);

			if (utils::lowerCase(stringOf(header)) == utils::lowerCase(headerName)) {
				return true;
			}
		}

		return false;
	}

	bool isValidHeaderField(const std::string &headerField) {
		std::regex headerFieldRegex(R"(^([a-zA-Z0-9!#$%&'*+.^_`|~-]+):\s*(.*)\s*$)");

		if (!std::regex_match(headerField, headerFieldRegex)) {
			return false;
		}

		std::string headerName = headerField.substr(0, headerField.find(":"));

		if (!hasHeaderName(headerName)) {
			return false;
		}

		return true;
	}

	std::size_t parseChunkSize(std::string chunkSizeLine) {
		std::size_t semicolonPos = chunkSizeLine.find(";");

		if (semicolonPos != std::string::npos) {
			chunkSizeLine = chunkSizeLine.substr(0, semicolonPos);
		}

		std::size_t chunkSize;
		std::istringstream stream(chunkSizeLine);

		if (!(stream >> std::hex >> chunkSize)) {
			throw std::invalid_argument("Failed to parse chunk size");
		}

		return chunkSize;
	}

	std::array<std::string, 3> parseRequestLine(const std::string &rawRequestHeader) {
		std::size_t pos = rawRequestHeader.find("\r\n");

		if (pos == std::string::npos) {
			throw std::invalid_argument("Couldn't find the request line");
		}

		std::string requestLine = rawRequestHeader.substr(0, pos);
		std::regex requestLineRegex(R"(^(GET|POST|PUT|DELETE|HEAD|OPTIONS|PATCH|TRACE|CONNECT) (\S+) HTTP\/1\.1$)");
		std::smatch matches;

		if (!std::regex_match(requestLine, matches, requestLineRegex)) {
			throw std::invalid_argument("Malformed or invalid request line");
		}

		std::istringstream istream(requestLine);
		std::string method;
	 	std::string uri;
	 	std::string version;

		istream >> method >> uri >> version;
		return { method, uri, version };
	}

	std::unordered_map<std::string, std::string> parseRequestHeaders(const std::string &rawRequestHeader) {
		std::size_t pos = rawRequestHeader.find("\r\n");
		std::unordered_map<std::string, std::string> headerByName;
		std::istringstream istream(rawRequestHeader.substr(pos + 2));
		std::string line;

		while (std::getline(istream, line) && line != "\r") {
			if (!isValidHeaderField(line)) {
				continue;
			}

			std::size_t colonPos = line.find(":");
			std::string name = line.substr(0, colonPos);
			std::string header = utils::trimSpace(line.substr(colonPos + 1));
			headerByName[name] = header;
		}

		if (headerByName.find(stringOf(Header::HOST)) == headerByName.end()) {
			throw std::invalid_argument("No Host found in header request");
		}

		return headerByName;
	}
}
