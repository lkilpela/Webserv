#pragma once

#include <string>

namespace http {

	enum class Mime {

	};

	constexpr const char* getMimeType(const std::string& extension);
}
