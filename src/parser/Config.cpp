#include "Config.hpp"
#include "Utils.hpp"
#include "Error.hpp"
#include <functional> // std::function
#include <fstream> // std::ifstream, std::getline
#include <iostream> // std::cout, std::endl
#include <sstream> // std::istringstream
#include <string> // std::string
#include <stdexcept> // std::invalid_argument, std::out_of_range
#include <unordered_map> // std::unordered_map
#include <algorithm> // std::find

// Define namespaces
using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::cout;
using std::endl;

// Define types for parsers
using ParserFunction = std::function<void(const string&)>;
using ParserMap = std::unordered_map<string, ParserFunction>;

// Function to parse global configuration lines
void ConfigParser::parseGlobal(const string &line, ServerConfig &config) {
    static const ParserMap globalParsers = {
        {"host", [&](const string &value) {
            if (!utils::isValidIP(value)) {
                throw ConfigException("Invalid IP address: " + value, std::errc::invalid_argument);
            }
            config.host = value;
        }},
        {"port", [&](const string &value) {
            config.port = utils::parsePort(value);
        }},
        {"server_name", [&](const string &value) {
            config.serverName = value;
        }},
        {"error_page", [&](const string &value) {
            istringstream iss(value);
            std::string code, path;
            iss >> code >> path;
            utils::validateErrorPage(code, path);
            config.errorPages[std::stoi(code)] = path; // Store in map
        }},
        {"client_max_body_size", [&](const string &value) {
            if (!utils::isValidSize(value)) {
                throw ConfigException("Invalid size format: " + value, std::errc::invalid_argument);
            }
            config.clientMaxBodySize = value;
        }}
    };

    istringstream iss(line);
    string key, value;
    if (iss >> key) {
        getline(iss, value);
        value = utils::trim(utils::removeComments(value));
        auto it = globalParsers.find(key);
        if (it != globalParsers.end()) {
            it->second(value);
        } else {
            throw ConfigException("Unknown configuration key: " + key, std::errc::invalid_argument);
        }
    }
}

void ConfigParser::parseLocation(const string &line, Location &currentLocation) {
    static const ParserMap locationParsers = {
        {"root", [&](const string &value) {
            if (!utils::isValidFilePath(value)) {
                throw ConfigException("Invalid file path: " + value, std::errc::invalid_argument);
            }
            currentLocation.root = value;
        }},
        {"index", [&](const string &value) {
            string fullPath = currentLocation.root + "/" + value;
            if (!utils::isValidFilePath(fullPath)) {
                throw ConfigException("Invalid file path: " + fullPath, std::errc::invalid_argument);
            }
            currentLocation.index = value;
        }},
        {"autoindex", [&](const string &value) {
            currentLocation.isAutoIndex = utils::parseBool(value);
        }},
        {"methods", [&](const string &value) {
            istringstream iss(value);
            vector<string> methods;
            string method;
            while (iss >> method) {
                methods.push_back(method);
            }
            utils::validateMethods(methods);
            currentLocation.methods = methods;
        }},
        {"cgi_extension", [&](const string &value) {
            currentLocation.cgiExtension = value;
        }},
        {"upload_dir", [&](const string &value) {
            if (!utils::isValidFilePath(value)) {
                throw ConfigException("Invalid file path: " + value, std::errc::invalid_argument);
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
            if (returnParts.size() != 2 || !utils::isValidURL(returnParts[1])) {
                throw ConfigException("Invalid return directive: " + value, std::errc::invalid_argument);
            }
            currentLocation.returnUrl = returnParts;
        }}
    };

    istringstream iss(line);
    string key, value;
    if (iss >> key) {
        getline(iss, value);
        value = utils::trim(utils::removeComments(value));
        auto it = locationParsers.find(key);
        if (it != locationParsers.end()) {
            it->second(value);
        }
    }
}

void ConfigParser::parseConfig(const string &filename, Config& config) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw ConfigException("Failed to open file: " + filename, std::errc::no_such_file_or_directory);
    }
    string line;
    ServerConfig currentServer;
    Location currentLocation;
    bool inHttpBlock = false;
    bool inServerBlock = false;
    bool inLocationBlock = false;

    while (std::getline(file, line)) {
        line = utils::trim(utils::removeComments(line));
        if (line.empty()) continue;

        // Start of the http block
        if (line == "http {") {
            if (inHttpBlock) {
                throw ConfigException("Nested http blocks are not allowed.", std::errc::invalid_argument);
            }
            inHttpBlock = true;
            continue;
        }

        // End of the http block
        if (line == "}") {
            if (inLocationBlock) {
                currentServer.locations.push_back(currentLocation);
                currentLocation = Location(); // Reset location
                inLocationBlock = false;
            } else if (inServerBlock) {
                config.servers.push_back(currentServer);
                currentServer = ServerConfig(); // Reset server
                inServerBlock = false;
            } else if (inHttpBlock) {
                inHttpBlock = false;
            } else {
                throw ConfigException("Unmatched closing brace.", std::errc::invalid_argument);
            }
            continue;
        }

        // Start of a server block
        if (line == "server {") {
            if (!inHttpBlock) {
                throw ConfigException("Server block outside of http block.", std::errc::invalid_argument);
            }
            if (inServerBlock) {
                throw ConfigException("Nested server blocks are not allowed.", std::errc::invalid_argument);
            }
            inServerBlock = true;
            currentServer = ServerConfig(); // Initialize a new server block
            continue;
        }

        // Start of a location block
        if (line.find("location ") == 0) {
            if (!inServerBlock) {
                throw ConfigException("Location block outside of server block.", std::errc::invalid_argument);
            }
            if (inLocationBlock) {
                throw ConfigException("Nested location blocks are not allowed.", std::errc::invalid_argument);
            }
            inLocationBlock = true;
            istringstream iss(line);
            string key, path;
            iss >> key >> path;
            currentLocation = Location(); // Initialize a new location block
            currentLocation.path = path;
            continue;
        }

        if (inLocationBlock) {
            parseLocation(line, currentLocation);
        } else if (inServerBlock) {
            parseGlobal(line, currentServer);
        // Invalid directive outside of recognized blocks
        } else if (inHttpBlock) {
            throw ConfigException("Unexpected directive inside http block: " + line, std::errc::invalid_argument);
        } else {
            throw ConfigException("Unexpected directive outside of any block: " + line, std::errc::invalid_argument);
        }
    }

    // Final validations for unclosed blocks
    if (inHttpBlock) {
       ConfigException("Unclosed http block.", std::errc::invalid_argument);
    }
    if (inServerBlock) {
        ConfigException("Unclosed server block.", std::errc::invalid_argument);
    }
    if (inLocationBlock) {
        ConfigException("Unclosed location block.", std::errc::invalid_argument);
    }
}

// Ultility function to print the server configuration
void printServerConfig(const ServerConfig& server) {
    cout << "Server Config: " << endl;
    cout << "Host: " << server.host << endl;
    cout << "Port: " << server.port << endl;
    cout << "Server Name: " << server.serverName << endl;
    cout << "Error Pages: " << endl;
    for (const auto& [code, path] : server.errorPages) {
        cout << code << ": " << path << " " << endl;
    }
    cout << endl;
    cout << "Client Max Body Size: " << server.clientMaxBodySize << endl;

    for (const auto& location : server.locations) {
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
    cout << "----------------------------------------" << endl;
}

// Function to print the configuration
void ConfigParser::printConfig(const Config& config) {
    for (const auto& server : config.servers) {
        printServerConfig(server);
    }
}

// Function to load the configuration
void ConfigParser::load(const string& filePath) {
    Config config;
    ConfigParser parser;
    parser.parseConfig(filePath, config);
    parser.printConfig(config);
}
