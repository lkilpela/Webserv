#include "Utils.hpp"
#include <regex> // std::regex, std::regex_match
#include <filesystem> // std::filesystem
#include <stdexcept> // std::invalid_argument, std::out_of_range

using std::string;
using std::vector;

namespace utils {

    // Utility functions for validation
    bool parseBool(const string &value) {
        if (value == "on") return true;
        if (value == "off") return false;
        throw std::invalid_argument("Invalid boolean value: " + value);
    }

    int parsePort(const string &value) {
        std::regex port_regex("^[0-9]+$");
        if (!std::regex_match(value, port_regex)) {
            throw std::invalid_argument("Invalid port number: " + value);
        }

        int port = std::stoi(value);
        if (port <= 0 || port > 65535) {
            throw std::out_of_range("Port number out of range: " + value);
        }
        return port;
    }

    void validateMethods(const vector<string> &methods) {
        vector<string> validMethods = {"GET", "POST", "DELETE"};
        for (const auto &method : methods) {
            if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end()) {
                throw std::invalid_argument("Invalid HTTP method: " + method);
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
            R"(\d+[KMG]?)");
        return std::regex_match(size, sizePattern);
    }

    void validateErrorPage(const string &code, const string &path) {
        std::regex code_regex("^[1-5][0-9][0-9]$");
        if (!std::regex_match(code, code_regex)) {
            throw std::invalid_argument("Invalid error code: " + code);
        }
        if (!isValidFilePath(path)) {
            throw std::invalid_argument("Invalid file path: " + path);
        }
    }

    // Utility functions for parsing
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t;");
        size_t last = str.find_last_not_of(" \t;");
        return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
    }

    string removeComments(const std::string& str) {
    size_t commentPos = str.find('#');
    if (commentPos != string::npos) {
            return str.substr(0, commentPos);
        }
        return str;
    }
	template <typename T>
	bool isInVector(T element, std::vector<T>& v){
		return std::find(v.begin(), v.end(), element) != v.end();
	}
} // namespace utils