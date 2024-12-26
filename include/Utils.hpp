#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

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
	int setNonBlocking(int fd);
	void closeFDs(std::vector<int> vector);
	
	template <typename T>
	bool isInVector(const T& element, const std::vector<T>& v) {
		return std::find(v.begin(), v.end(), element) != v.end();
	}
} // namespace utils
