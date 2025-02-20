#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include "constants.hpp"

namespace http {
	std::string getMimeType(const std::string &extension);
	std::string getExtensionFromMimeType(const std::string& mime);
	std::string stringOf(Header header);
	std::string stringOf(StatusCode code);

	bool hasHeaderName(const std::string &headerName);
	bool isValidHeaderField(const std::string &headerField);
}
