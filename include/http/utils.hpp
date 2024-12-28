#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include "constants.hpp"

namespace http {
	constexpr const char* getMimeType(const std::string& extension);
	constexpr const char* stringOf(Header header);
	constexpr const char* stringOf(StatusCode code);

	/**
	 * @brief Searches for the header-body delimiter in an HTTP request.
	 *
	 * Searches for the header-body delimiter (`\r\n\r\n`) in a range of bytes,
	 * typically representing an HTTP request.
	 *
	 * @tparam Iterator Type of the iterator used to traverse the input range. It must satisfy
	 *                  the requirements of an input iterator.
	 *
	 * @param begin The starting iterator of the range to search.
	 * @param end The ending iterator of the range to search.
	 *
	 * @return std::size_t	The position of the first byte after the header-body delimiter,
	 * 						otherwise `std::string::npos`.
	 *
	 */
	template <typename Iterator>
	std::size_t findBlankLine(const Iterator begin, const Iterator end) {
		static constexpr std::vector<std::uint8_t> delimiter = {'\r', '\n', '\r', '\n'};
		auto pos = std::search(begin, end, delimiter.begin(), delimiter.end());

		if (pos == end) {
			return std::string::npos;
		}
		
		return std::distance(begin, pos) + delimiter.size();
	}
}
