#include <regex>
#include "http/Url.hpp"

namespace http {
	Url Url::parse(const std::string& url) {
		Url result;

		std::regex urlRegex(
			R"(^(?:(https?)://)?" // Scheme
			R"((?:([^:@]+)(?::([^@]+))?@)?)" // User and Password
			R"(([^:/?#]+))" // Host
			R"(?::(\d+))?" // Port
			R"((/[^?#]*)?)" // Path
			R"(\??([^#]*))?" // Query
			R"(#?(.*)?)$)", // Fragment
			std::regex::extended
    	);
		std::smatch matches;

		if (std::regex_match(url, matches, urlRegex)) {
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
}