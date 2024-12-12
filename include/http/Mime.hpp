#pragma once

#include <string>

namespace http {

	enum class Mime {

	};

	static constexpr const char* getMimeType(const std::string& extension);
}
