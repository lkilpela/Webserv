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
#include "http/Request.hpp"
#include "http/Response.hpp"

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

	// Test valid case: root = /Users/username/Webserv/config, path = /static/index.html
	// Expected: /Users/username/Webserv/config/static/index.html
	// Test invalid case: root = /Users/username/Webserv/config, path = /../index.html -> fullPath = /Users/username/Webserv/index.html
	// Test invalid case: root = /Users/username/Webserv/config, path = /static/../index.html -> fullPath = /Users/username/Webserv/index.html
	// Test invalid case: root = /Users/username/Webserv/config, path = /static/../../index.html -> fullPath = /Users/username/index.html
	// Test invalid case: root = /Users/username/Webserv/config, path = /static/../../../index.html -> fullPath = /Users/index.html
	// Expected: ConfigError exception
	std::string sanitizePath(const std::string& root, const std::string& path) {
		std::cout << "Sanitizing path. Root: " << root << ", URL Path: " << path << std::endl;
		std::filesystem::path fullPath;
		if (path.front() == '/') {
			fullPath = std::filesystem::canonical(root + path);
		} else {
			fullPath = std::filesystem::canonical(root + "/" + path);
		}
		std::filesystem::path rootPath = std::filesystem::canonical(root);
		std::cout << "Full path after canonical: " << fullPath << std::endl;
		if (fullPath.string().find(rootPath.string()) != 0) {
			throw ConfigError(EINVAL, "Path traversal attempt detected");
		}
		return fullPath.string();
	}

	void printRequest(const http::Request& request) {
		std::cout << "Request method: " << request.getMethod() << std::endl;
		std::cout << "Request URL_scheme: " << request.getUrl().scheme << std::endl;
		std::cout << "Request URL_user: " << request.getUrl().user << std::endl;
		std::cout << "Request URL_password: " << request.getUrl().password<< std::endl;
		std::cout << "Request URL_host: " << request.getUrl().host << std::endl;
		std::cout << "Request URL_port: " << request.getUrl().port << std::endl;
		std::cout << "Request URL_path: " << request.getUrl().path << std::endl;
		std::cout << "Request URL_query: " << request.getUrl().query << std::endl;
		std::cout << "Request URL_fragment: " << request.getUrl().fragment << std::endl;
		std::cout << "Request version: " << request.getVersion() << std::endl;
		std::cout << "Request body size: " << request.getBodySize() << std::endl;
	}
} // namespace utils
