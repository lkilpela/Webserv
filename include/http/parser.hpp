#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "data_types.hpp"
#include "Request.hpp"

namespace http {
	Url parseUrl(const std::string& fullUrl);

	void parseRequestLine(const std::string& requestLine, Request& request);
	void parseRequestHeaderFields(const std::string &headerFields, Request& request);
	void parseRequestHeader(std::vector<uint8_t>& buffer, Request& request);
	
	void parseRequestBody(std::vector<uint8_t>& buffer, Request& request);
	std::size_t parseChunkSize(std::string chunkSizeLine);
	bool parseChunkRequestBody(std::vector<uint8_t>& chunkedBuffer, std::vector<uint8_t>& result);

	std::vector<MultipartElement> parseMultipart(const std::vector<uint8_t>& rawBody, const std::string& boundary);
}
