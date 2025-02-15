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

	Url parseUrl(const std::string& fullUrl) {
		Url result;

		std::regex urlRegex(R"((https?://)?(?:([^:@]+)(?::([^:@]*))?@)?([^:/?#]+)(?::(\d+))?(/[^?#]*)?(?:\?([^#]*))?(?:#(.*))?)");
		std::smatch matches;

		if (!std::regex_match(fullUrl, matches, urlRegex)) {
			throw std::invalid_argument("Invalid URL");
		}

		result.scheme = matches[1].str();
		result.user = matches[2].str();
		result.password = matches[3].str();
		result.host = matches[4].str();
		result.port = matches[5].str();
		result.path = matches[6].str();
		result.query = matches[7].str();
		result.fragment = matches[8].str();
		return result;
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

	std::unordered_map<std::string, std::string> parseRequestHeaderFields(const std::string &rawRequestHeader) {
		std::size_t pos = rawRequestHeader.find("\r\n");
		std::unordered_map<std::string, std::string> headerByName;
		std::istringstream istream(rawRequestHeader.substr(pos + 2));
		std::string line;

		while (std::getline(istream, line) && line != "\r") {
			if (!isValidHeaderField(line)) {
				continue;
			}

			std::size_t colonPos = line.find(":");
			std::string headerName = line.substr(0, colonPos);
			std::string headerValue = utils::trimSpace(line.substr(colonPos + 1));
			headerByName[headerName] = headerValue;
		}

		// if (name == stringOf(Header::CONTENT_LENGTH)) {
		// 		request.setContentLength(std::stoul(value));
		// 	}

		if (headerByName.find(stringOf(Header::HOST)) == headerByName.end()) {
			throw std::invalid_argument("No Host found in header request");
		}

		return headerByName;
	}

	std::size_t parseChunkSize(std::string chunkSizeLine) {
		std::size_t semicolonPos = chunkSizeLine.find(";");

		if (semicolonPos != std::string::npos) {
			chunkSizeLine = chunkSizeLine.substr(0, semicolonPos);
		}

		std::size_t chunkSize;
		std::istringstream stream(chunkSizeLine);

		if (!(stream >> std::hex >> chunkSize)) {
			throw std::invalid_argument("Invalid chunk size");
		}

		return chunkSize;
	}

	bool unchunk(std::vector<uint8_t>& chunkedBuffer, std::vector<uint8_t>& result) {
		bool isChunkEnd = false;
		auto begin = chunkedBuffer.begin();
		auto end = chunkedBuffer.end();
		auto currentPos = begin;

		result.reserve(chunkedBuffer.size());

		while (true) {
			auto delimPos = utils::findDelimiter(currentPos, end, {'\r', '\n'});

			if (delimPos == end || std::distance(delimPos, end) < 2) {
				break;
			}

			std::size_t chunkSize = parseChunkSize(std::string(currentPos, delimPos));
			std::size_t distance = static_cast<std::size_t>(std::distance(delimPos + 2, end));

			if (distance < chunkSize + 2) {
				break;
			}

			currentPos = delimPos + 2;

			if (chunkSize == 0) {
				if (*currentPos == '\r' && *(currentPos + 1) == '\n') {
					isChunkEnd = true;
					currentPos += 2;
					break;
				}

				throw std::invalid_argument(R"(Error: Chunk body did not end with 0\r\n\r\n)");
			}

			result.insert(result.end(), currentPos, currentPos + chunkSize);
			currentPos += chunkSize + 2;
		}

		chunkedBuffer.erase(begin, currentPos);
		return isChunkEnd;
	}
}
