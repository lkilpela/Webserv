#pragma once

#include <string>

namespace utils {
    std::string trim(const std::string& str);
    std::string removeComments(const std::string& str);
    bool parseBool(const std::string &value);
    int parsePort(const std::string &value);
    void validateMethods(const std::vector<std::string> &methods);
    bool isValidIP(const std::string &ip);
    bool isValidFilePath(const std::string &path);
    bool isValidURL(const std::string &url);
    bool isValidSize(const std::string &size);
    void validateErrorPage(const std::string &code, const std::string &path);

	constexpr const char* getMimeType(const std::string& extension);

	/**
	 * @brief Searches for the header-body delimiter in an HTTP request.
	 *
	 * This function searches for the header-body delimiter (`\r\n\r\n`) in a range of bytes,
	 * typically representing an HTTP request.
	 *
	 * @tparam Iterator Type of the iterator used to traverse the input range. It must satisfy
	 *                  the requirements of an input iterator.
	 *
	 * @param begin The starting iterator of the range to search.
	 * @param end The ending iterator of the range to search.
	 *
	 * @return std::size_t The position of the first byte after the header-body delimiter.
	 *
	 * @throws RequestException If the delimiter (`\r\n\r\n`) is not found in the given range.
	 *                          This indicates that the HTTP request is malformed.
	 */
	template<typename Iterator>
	std::size_t findBlankLine(const Iterator begin, const Iterator end) {
		static constexpr std::vector<std::uint8_t> delimiter = {'\r', '\n', '\r', '\n'};
		auto pos = std::search(begin, end, delimiter.begin(), delimiter.end());
		if (pos == end) {
			throw RequestException("Bad Request: Missing header-body delimiter");
		}
		return std::distance(begin, pos) + delimiter.size();
	}

} // namespace utils