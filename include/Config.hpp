#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

# define YELLOW "\033[0;33m"
# define RESET "\033[0m"

// Server struct
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
    map<int, string> errorPages;
    string clientMaxBodySize;
    vector<Location> locations;
};

// Multi servers struct
struct Config {
    vector<ServerConfig> servers;
};

class ConfigParser {
public:
    // Function to parse the configuration text
    void parseConfig(const string &filename, Config &config);
    void parseGlobal(const string &line, ServerConfig &config);
    void parseLocation(const string &line, Location &currentLocation);
	void load(const string& filePath);
	void printConfig(const Config& config);
};
