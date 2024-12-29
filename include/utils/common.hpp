#pragma once

#include <vector>
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

	template <typename T>
	void move(std::vector<T>& dest, std::vector<T>& src, std::size_t n) {
		if (n > src.size()) {
			throw std::out_of_range("src vector has less bytes to move");
		}

		dest.insert(
			dest.end(),
			std::move_iterator(src.begin()),
			std::move_iterator(src.begin() + n)
		);
		src.erase(src.begin(), src.begin() + n);
	};
}
