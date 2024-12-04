#pragma once

#include <string>
#include <unordered_map>

namespace utils {
	class HttpRequestParser {
		public:
			static std::unordered_map<std::string, std::string> parseHeaders(std::string& rawHeader);
	};
}
