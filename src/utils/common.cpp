#include "utils/common.hpp"
#include "Error.hpp"
#include <regex> // std::regex, std::regex_match
#include <filesystem> // std::filesystem
#include <stdexcept> // std::invalid_argument, std::out_of_range
#include <iostream>
#include <algorithm> // std::find
#include <cctype> // std::tolower
#include <fcntl.h> // fcntl
#include <unistd.h> // close

using std::string;
using std::vector;

namespace utils {
	bool parseBool(const string &value) {
		if (value == "on") {
			return true;
		}

		if (value == "off") {
			return false;
		}

		throw ConfigError(EINVAL, "Invalid boolean value");
	}

	int parsePort(const string &value) {
		std::regex port_regex("^[0-9]+$");

		if (!std::regex_match(value, port_regex)) {
			throw ConfigError(EINVAL, "Invalid port number");
		}

		int port = std::stoi(value);

		if (port <= 0 || port > 65535) {
			throw ConfigError(ERANGE, "Port number out of range");
		}

		return port;
	}

	void validateMethods(const vector<string> &methods) {
		vector<string> validMethods = {"GET", "POST", "DELETE"};

		for (const auto &method : methods) {
			if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end()) {
				throw ConfigError(EINVAL, "Invalid method");
			}
		}
	}

	bool isValidIP(const string &ip) {
		std::regex ipPattern(
			R"((\d{1,3}\.){3}\d{1,3})");
		return std::regex_match(ip, ipPattern);
	}

	bool isValidFilePath(const string &path) {
		return std::filesystem::exists(path);
	}

	bool isValidURL(const string &url) {
		std::regex urlPattern(
			R"((http|https)://([^\s/$.?#].[^\s]*)$)");
		return std::regex_match(url, urlPattern);
	}

	bool isValidSize(const string &size) {
		std::regex sizePattern(
			R"(\d+([KMGkmg])?)");
		return std::regex_match(size, sizePattern);
	}

	void validateErrorPage(const string &code, const string &path) {
		std::regex code_regex("^[1-5][0-9][0-9]$");

		if (!std::regex_match(code, code_regex)) {
			throw ConfigError(EINVAL, "Invalid error code");
		}

		if (!isValidFilePath(path)) {
			throw ConfigError(EINVAL, "Invalid error page path");
		}
	}

	string trim(const string& str) {
		size_t first = str.find_first_not_of(" \t;");
		size_t last = str.find_last_not_of(" \t;");
		return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
	}

	std::string trimSpace(const std::string& str) {
		// Find the first non-space character
		auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
			return std::isspace(ch);
		});

		// Find the last non-space character
		auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
			return std::isspace(ch);
		}).base();

		// Create a substring without leading and trailing spaces
		return (start < end) ? std::string(start, end) : std::string();
	}

	string removeComments(const std::string& str) {
		size_t commentPos = str.find('#');

		if (commentPos != string::npos) {
			return str.substr(0, commentPos);
		}

		return str;
	}

	std::string lowerCase(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
			return std::tolower(c);
		});

		return str;
	}

	std::size_t convertSizeToBytes(const std::string& sizeStr) {
		std::size_t size = std::stoul(sizeStr);

		// handle lowercase and uppercase K, M, G
		switch (sizeStr.back()) {
			case 'K':
			case 'k':
				size *= 1024;
				break;
			case 'M':
			case 'm':
				size *= 1024 * 1024;
				break;
			case 'G':
			case 'g':
				size *= 1024 * 1024 * 1024;
				break;
			default:
				break;
		}
		return size;
	}

	int setNonBlocking(int fd) {
		int flags = fcntl(fd, F_GETFL, 0);
		if (flags == -1) {
			return -1;
		}
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	//isInVector
	template <typename T>
	bool isInVector(const T &value, const std::vector<T> &vec) {
		return std::find(vec.begin(), vec.end(), value) != vec.end();
	}
	
	void closeFDs(const std::vector<int> &fds) {
		for (const auto &fd : fds) {
			close(fd);
		}
	}
} // namespace utils
