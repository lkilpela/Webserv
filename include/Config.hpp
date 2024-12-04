#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

# define YELLOW "\033[0;33m"
# define RESET "\033[0m"

struct Location {
    string path;
    string root;
    string index;
    bool isAutoIndex;
    vector<string> methods;
    string cgiExtension;
    string uploadDir;
    vector<string> returnUrl;
};

struct ServerConfig {
    string host;
    unsigned int port;
    string serverName;
    vector<string> errorPages;
    string clientMaxBodySize;
    vector<Location> locations;
};

class ConfigParser {
private:
    // Helper functions
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

public:

	ConfigParser() = default;

    // Function to parse the configuration text
    void parseConfig(const string &filename, ServerConfig &config);
    void parseGlobalConfig(const string &line, ServerConfig &config);
    void parseLocationConfig(const string &line, Location &currentLocation);
	void load(const string& filePath);
	void printConfig(const ServerConfig& config);
};
