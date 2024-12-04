#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>

// Define namespaces
using std::ifstream;
using std::istringstream;
using std::cout;
using std::endl;

// Define types for parsers
using ParserFunction = std::function<void(const string&)>;
using ParserMap = std::unordered_map<string, ParserFunction>;

// Utility functions for validation
bool parseBool(const string &value) {
    if (value == "on") return true;
    if (value == "off") return false;
    throw std::invalid_argument("Invalid boolean value: " + value);
}

int parsePort(const string &value) {
    int port = std::stoi(value);
    if (port <= 0 || port > 65535) {
        throw std::out_of_range("Port number out of range: " + value);
    }
    return port;
}

void validateMethods(const std::vector<string> &methods) {
    std::vector<string> validMethods = {"GET", "POST", "PUT", "DELETE"};
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
    // Basic validation for file paths (can be extended)
    return !path.empty() && path[0] != ' ';
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

// Function to parse global configuration lines
void ConfigParser::parseGlobalConfig(const string &line, ServerConfig &config) {
    static const ParserMap globalParsers = {
        {"host", [&](const string &value) {
            if (!isValidIP(value)) {
                throw std::invalid_argument("Invalid IP address: " + value);
            }
            config.host = value;
        }},
        {"port", [&](const string &value) {
            config.port = parsePort(value);
        }},
        {"server_name", [&](const string &value) {
            config.serverName = value;
        }},
        {"error_page", [&](const string &value) {
            istringstream iss(value);
            vector<string> errorPage;
            string part;
            while (iss >> part) {
                errorPage.push_back(part);
            }
            if (errorPage.size() != 2|| !isValidFilePath(errorPage[1])) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            config.errorPages = errorPage;
        }},
        {"client_max_body_size", [&](const string &value) {
            if (!isValidSize(value)) {
                throw std::invalid_argument("Invalid size format: " + value);
            }
            config.clientMaxBodySize = value;
        }}
    };

    istringstream iss(line);
    string key, value;
    if (iss >> key) {
        getline(iss, value);
        value = trim(removeComments(value));
        auto it = globalParsers.find(key);
        if (it != globalParsers.end()) {
            it->second(value);
        }
    }
}

void ConfigParser::parseLocationConfig(const string &line, Location &currentLocation) {
    static const ParserMap locationParsers = {
        {"root", [&](const string &value) {
            if (!isValidFilePath(value)) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            currentLocation.root = value;
        }},
        {"index", [&](const string &value) {
            if (!isValidFilePath(value)) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            currentLocation.index = value;
        }},
        {"autoindex", [&](const string &value) {
            currentLocation.isAutoIndex = parseBool(value);
        }},
        {"methods", [&](const string &value) {
            istringstream iss(value);
            vector<string> methods;
            string method;
            while (iss >> method) {
                methods.push_back(method);
            }
            validateMethods(methods);
            currentLocation.methods = methods;
        }},
        {"cgi_extension", [&](const string &value) {
            currentLocation.cgiExtension = value;
        }},
        {"upload_dir", [&](const string &value) {
            if (!isValidFilePath(value)) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            currentLocation.uploadDir = value;
        }},
        {"return", [&](const string &value) {
            istringstream iss(value);
            vector<string> returnParts;
            string part;
            while (iss >> part) {
                returnParts.push_back(part);
            }
            if (returnParts.size() != 2 || !isValidURL(returnParts[1])) {
                throw std::invalid_argument("Invalid return directive: " + value);
            }
            currentLocation.returnUrl = returnParts;
        }}
    };

    istringstream iss(line);
    string key, value;
    if (iss >> key) {
        getline(iss, value);
        value = trim(removeComments(value));
        auto it = locationParsers.find(key);
        if (it != locationParsers.end()) {
            it->second(value);
        }
    }
}

void ConfigParser::parseConfig(const string &filename, ServerConfig &config) {
    ifstream file(filename);
    string line;
    Location currentLocation;
    bool inLocationBlock = false;

    while (std::getline(file, line)) {
        line = trim(removeComments(line));
        if (line.empty()) continue;
        if (line.find("location") != string::npos) {
            if (inLocationBlock) {
                config.locations.push_back(currentLocation);
                currentLocation = Location();
            }
            inLocationBlock = true;
            istringstream iss(line);
            string key;
            iss >> key >> currentLocation.path;
            continue;
        }
        if (line.find('}') != string::npos) {
            if (inLocationBlock) {
                config.locations.push_back(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
            }
            continue;
        }
        try {
            if (inLocationBlock) {
                parseLocationConfig(line, currentLocation);
            } else {
                parseGlobalConfig(line, config);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error parsing line: " << line << "\n" << e.what() << "\n";
            exit(1);
        }
    }
    if (inLocationBlock) {
        config.locations.push_back(currentLocation);
    }
}

void ConfigParser::printConfig(const ServerConfig& config) {
    cout << "Host: " << config.host << endl;
    cout << "Port: " << config.port << endl;
    cout << "Server Name: " << config.serverName << endl;
    cout << "Error Page: ";
    for (const auto& page : config.errorPages) {
        cout << page << " ";
    }
    cout << endl;
    cout << "Client Max Body Size: " << config.clientMaxBodySize << endl;

    for (const auto& location : config.locations) {
        cout << "Location Path: " << location.path << endl;
        cout << "Root: " << location.root << endl;
        cout << "Index: " << location.index << endl;
        cout << "Autoindex: " << (location.isAutoIndex ? "on" : "off") << endl;
        cout << "Methods: ";
        for (const auto& method : location.methods) {
            cout << method << " ";
        }
        cout << endl;
        if (!location.cgiExtension.empty()) {
            cout << "CGI Extension: " << location.cgiExtension << endl;
        }
        if (!location.uploadDir.empty()) {
            cout << "Upload Dir: " << location.uploadDir << endl;
        }
        if (!location.returnUrl.empty()) {
            cout << "Return URL: ";
            for (const auto& part : location.returnUrl) {
                cout << part << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void ConfigParser::load(const string& filePath) {
    ServerConfig config;
    parseConfig(filePath, config);
    printConfig(config);
}
