#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "data_types.hpp"
#include "Request.hpp"

namespace http {
	Url parseUrl(const std::string& fullUrl);

	void parseRequestHeader(std::vector<uint8_t>& buffer, Request& request);
	void parseRequestBody(std::vector<uint8_t>& buffer, Request& request, std::size_t clientMaxBodySize);

	std::vector<MultipartElement> parseMultipart(const std::vector<uint8_t>& rawBody, const std::string& boundary);
}
