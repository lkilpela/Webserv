#include <iostream>
#include <regex>
#include "http/Url.hpp"

namespace http {
	Url Url::parse(const std::string& url) {
		Url result;

		std::regex urlRegex(R"((https?://)?(?:([^:@]+)(?::([^:@]*))?@)?([^:/?#]+)(?::(\d+))?(/[^?#]*)?(?:\?([^#]*))?(?:#(.*))?)");
		std::smatch matches;

		if (!std::regex_match(url, matches, urlRegex)) {
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

	std::ostream &operator<<(std::ostream &ostream, const Url &url) {
		ostream << url.scheme << url.user << url.password << url.host << url.port << url.path << url.query << url.fragment;
		return ostream;
	}
}
