#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include "constants.hpp"

namespace http {
	constexpr const char* getMimeType(const std::string &extension);
	constexpr const char* stringOf(Header header);
	constexpr const char* stringOf(StatusCode code);

	bool hasHeaderName(const std::string &headerName);
	bool isValidHeaderField(const std::string &headerField);
	
	std::array<std::string, 3> parseRequestLine(const std::string &rawRequestLine);
	std::unordered_map<std::string, std::string> parseRequestHeaders(const std::string &rawRequestHeader);
}
