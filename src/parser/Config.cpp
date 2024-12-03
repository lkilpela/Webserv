#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>

using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

bool isValidIpAddress(const std::string& ip) {
    std::regex ipRegex(
        R"((\d{1,3}\.){3}\d{1,3})"
    );
    return std::regex_match(ip, ipRegex);
}

bool isValidPort(const std::string& port) {
    int portNum = std::stoi(port);
    return portNum > 0 && portNum <= 65535;
}

// Function to parse the configuration text
ServerConfig ConfigParser::parse(const string& filename) {
    ServerConfig config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::string line;
    Location currentLocation;
    bool inLocationBlock = false;

    while (std::getline(file, line)) {
        line = trim(removeComments(line));
        if (line.empty() || isComment(line)) continue;
        
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "host") {
            iss >> config.host;
        } else if (key == "port") {
            iss >> config.port;
        } else if (key == "server_name") {
            iss >> config.server_name;
        } else if (key == "error_page_404" || key == "error_page_500") {
            std::string value;
            iss >> value;
            if (key == "error_page_404") {
                config.error_page_404 = value;
            } else {
                config.error_page_500 = value;
            }
        } else if (key == "client_max_body_size") {
            iss >> config.client_max_body_size;
        } else if (key == "location") {
            if (inLocationBlock) {
                config.locations.push_back(currentLocation);
                currentLocation = Location();
            }
            iss >> currentLocation.path;
            inLocationBlock = true;
        } else if (key == "root") {
            iss >> currentLocation.root;
        } else if (key == "index") {
            iss >> currentLocation.index;
        } else if (key == "autoindex") {
            std::string value;
            iss >> value;
            currentLocation.autoindex = (value == "on");
        } else if (key == "methods") {
            std::string method;
            while (iss >> method) {
                currentLocation.methods.push_back(method);
            }
        }
    }

    if (inLocationBlock) {
        config.locations.push_back(currentLocation);
    }

    return config;
}

void ConfigParser::printConfig(const ServerConfig& config) {
    // Output parsed values for verification
    std::cout << "Listen: " << config.host << ":" << config.port << std::endl;
    std::cout << "Server Name: " << config.server_name << std::endl;
    std::cout << "Error Page 404: " << config.error_page_404 << std::endl;
    std::cout << "Error Page 500: " << config.error_page_500 << std::endl;
    std::cout << "Client Max Body Size: " << config.client_max_body_size << std::endl;

    for (const auto& location : config.locations) {
        std::cout << "Location Path: " << location.path << std::endl;
        std::cout << "Root: " << location.root << std::endl;
        std::cout << "Index: " << location.index << std::endl;
        std::cout << "Autoindex: " << (location.autoindex ? "on" : "off") << std::endl;
        std::cout << "Methods: ";
        for (const auto& method : location.methods) {
            std::cout << method << " ";
        }
        std::cout << std::endl;
    }
}

ConfigParser ConfigParser::load(const string& filePath) {
    ConfigParser parser;
    ServerConfig config = parser.parse(filePath);
    parser.printConfig(config);
    return parser;
}
