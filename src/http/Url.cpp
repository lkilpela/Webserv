#include <regex>
#include "http/Url.hpp"

namespace http {
	Url::Url(const std::string& url) {
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
		std::smatch matchedResult;

		if (std::regex_match(url, matchedResult, urlRegex)) {
			scheme = matchedResult[1].str();
			user = matchedResult[2].str();
			password = matchedResult[3].str();
			host = matchedResult[4].str();
			port = matchedResult[5].str();
			path = matchedResult[6].str();
			query = matchedResult[7].str();
			fragment = matchedResult[8].str();
		}
	}
}