#pragma once

#include <string>
#include <vector>

using std::string;
using std::pair;

# define YELLOW "\033[0;33m"
# define RESET "\033[0m"

struct Location {
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> methods;
};

struct ServerConfig {
    std::string listen;
    std::string server_name;
    std::string error_page_404;
    std::string error_page_500;
    size_t client_max_body_size;
    std::vector<Location> locations;
};

class ConfigParser {
private:
    // Helper functions
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
    }

    bool isComment(const string& line) {
        return line.find("#") == 0;
    }

    pair<string, string> parseDirective(const string& line) {
        size_t pos = line.find(' ');
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        return make_pair(trim(key), trim(value));
    }

    string extractLocationPath(const string& line) {
        size_t start = line.find(' ') + 1;
        size_t end = line.find(' ', start);
        return line.substr(start, end - start);
    }

public:

	ConfigParser() = default;

    // Function to parse the configuration text
    ServerConfig parse(const string& filename);
	static ConfigParser load(const std::string& filePath);
	void printConfig(const ServerConfig& config);
};
