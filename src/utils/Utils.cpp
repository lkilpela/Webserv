#include "Utils.hpp"
#include "Error.hpp"
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
            R"(\d+[KMG]?)");
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

	constexpr const char* getMimeType(const std::string &extension) {
		if (extension == "aac") return "audio/aac";
		if (extension == "abw") return "application/x-abiword";
		if (extension == "apng") return "image/apng";
		if (extension == "arc") return "application/x-freearc";
		if (extension == "avif") return "image/avif";
		if (extension == "avi") return "video/x-msvideo";
		if (extension == "bin") return "application/octet-stream";
		if (extension == "bmp") return "image/bmp";
		if (extension == "bz") return "application/x-bzip";
		if (extension == "bz2") return "application/x-bzip2";
		if (extension == "cda") return "application/x-cdf";
		if (extension == "csh") return "application/x-csh";
		if (extension == "css") return "text/css";
		if (extension == "csv") return "text/csv";
		if (extension == "doc") return "application/msword";
		if (extension == "epub") return "application/epub+zip";
		if (extension == "gz") return "application/gzip";
		if (extension == "gif") return "image/gif";
		if (extension == "htm" || extension == "html") return "text/html";
		if (extension == "jar") return "application/java-archive";
		if (extension == "jpeg" || extension == "jpg") return "image/jpeg";
		if (extension == "js") return "text/javascript";
		if (extension == "json") return "application/json";
		if (extension == "jsonld") return "application/ld+json";
		if (extension == "mid" || extension == "midi") return "audio/midi";
		if (extension == "mjs") return "text/javascript";
		if (extension == "mp3") return "audio/mpeg";
		if (extension == "mp4") return "video/mp4";
		if (extension == "mpeg") return "video/mpeg";
		if (extension == "oga") return "audio/ogg";
		if (extension == "ogv") return "video/ogg";
		if (extension == "ogx") return "application/ogg";
		if (extension == "opus") return "audio/ogg";
		if (extension == "otf") return "font/otf";
		if (extension == "png") return "image/png";
		if (extension == "pdf") return "application/pdf";
		if (extension == "php") return "application/x-httpd-php";
		if (extension == "rar") return "application/vnd.rar";
		if (extension == "rtf") return "application/rtf";
		if (extension == "sh") return "application/x-sh";
		if (extension == "svg") return "image/svg+xml";
		if (extension == "tar") return "application/x-tar";
		if (extension == "tif" || extension == "tiff") return "image/tiff";
		if (extension == "ts") return "video/mp2t";
		if (extension == "ttf") return "font/ttf";
		if (extension == "txt") return "text/plain";
		if (extension == "wav") return "audio/wav";
		if (extension == "weba") return "audio/webm";
		if (extension == "webm") return "video/webm";
		if (extension == "webp") return "image/webp";
		if (extension == "woff") return "font/woff";
		if (extension == "woff2") return "font/woff2";
		if (extension == "xhtml") return "application/xhtml+xml";
		if (extension == "xml") return "application/xml";
		if (extension == "zip") return "application/zip";
		if (extension == "7z") return "application/x-7z-compressed";
		return "application/unknown";
	}

} // namespace utils