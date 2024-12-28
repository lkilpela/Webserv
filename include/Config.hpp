#pragma once

#include <string>
#include <vector>
#include <map>

# define YELLOW "\033[0;33m"
# define RESET "\033[0m"

// Server struct
struct Location {
    std::string path;
    std::string root;
    std::string index;
    std::string autoIndex;
    bool isAutoIndex;
    std::vector<std::string> methods;
    std::string cgiExtension;
    std::string uploadDir;
    bool allowUpload;
    std::vector<std::string> returnUrl;
};

struct ServerConfig {
    std::string host;
    int port;
    std::string serverName;
    std::map<int, std::string> errorPages;
    std::string clientMaxBodySize;
    std::vector<Location> locations;
};

// Multi servers struct
struct Config {
    std::vector<int> ports;
    std::vector<ServerConfig> servers;
};

class ConfigParser {
public:
    // Function to parse the configuration text
    void parseConfig(const std::string &filename, Config &config);
    void parseGlobal(const std::string &line, ServerConfig &config);
    void parseLocation(const std::string &line, Location &currentLocation);
	Config load(const std::string& filePath);
	void printConfig(const Config& config);
};
