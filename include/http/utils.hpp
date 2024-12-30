#pragma once

#include <string>
#include "constants.hpp"

namespace http {
	constexpr const char* getMimeType(const std::string& extension);
	constexpr const char* stringOf(Header header);
	constexpr const char* stringOf(StatusCode code);
}
