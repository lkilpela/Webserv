#pragma once

#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include "http/Request.hpp"

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
	void closeFDs(const std::vector<int>& serverFds);
	std::size_t convertSizeToBytes(const std::string& sizeStr);
	std::string sanitizePath(const std::string& root, const std::string& path);

	template <typename Iterator>
	Iterator findDelimiter(
		const Iterator begin,
		const Iterator end,
		std::initializer_list<typename std::iterator_traits<Iterator>::value_type> delimiter
	) {
		return std::search(begin, end, delimiter.begin(), delimiter.end());
	}

	template <typename T>
	bool isInVector(const T& element, const std::vector<T>& vec) {
    	return std::find(vec.begin(), vec.end(), element) != vec.end();
	}


	// PRINT FUNCTIONS
	void printRequest(const http::Request& request);

}
