#pragma once

#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <iterator>

namespace utils {
    std::string trim(const std::string &str);
	std::string trimSpace(const std::string& str);
    std::string removeComments(const std::string &str);
	std::string lowerCase(std::string str);
    bool parseBool(const std::string &value);
    int parsePort(const std::string &value);
    void validateMethods(const std::vector<std::string> &methods);
    bool isValidIP(const std::string &ip);
    bool isValidFilePath(const std::string &path);
    bool isValidURL(const std::string &url);
    bool isValidSize(const std::string &size);
    void validateErrorPage(const std::string &code, const std::string &path);

	template <typename Iterator>
	Iterator findDelimiter(
		const Iterator begin,
		const Iterator end,
		std::initializer_list<typename std::iterator_traits<Iterator>::value_type> delimiter
	) {
		return std::search(begin, end, delimiter.begin(), delimiter.end());
	}
}
