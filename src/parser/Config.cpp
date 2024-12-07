#include "Config.hpp"
#include "Utils.hpp"
#include <fstream> // std::ifstream, std::getline
#include <iostream> // std::cout, std::endl
#include <sstream> // std::istringstream
#include <string> // std::string
#include <stdexcept> // std::invalid_argument, std::out_of_range
#include <unordered_map> // std::unordered_map
#include <algorithm> // std::find

// Define namespaces
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
                throw std::invalid_argument("Invalid IP address: " + value);
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
                throw std::invalid_argument("Invalid size format: " + value);
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
            throw std::invalid_argument("Unknown configuration key: " + key);
        }
    }
}

void ConfigParser::parseLocation(const string &line, Location &currentLocation) {
    static const ParserMap locationParsers = {
        {"root", [&](const string &value) {
            if (!utils::isValidFilePath(value)) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            currentLocation.root = value;
        }},
        {"index", [&](const string &value) {
            string fullPath = currentLocation.root + "/" + value;
            if (!utils::isValidFilePath(fullPath)) {
                throw std::invalid_argument("Invalid file path: " + fullPath);
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
            if (returnParts.size() != 2 || !utils::isValidURL(returnParts[1])) {
                throw std::invalid_argument("Invalid return directive: " + value);
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
        throw std::runtime_error("Failed to open file: " + filename);
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
                throw std::runtime_error("Nested http blocks are not allowed.");
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
                throw std::runtime_error("Unmatched closing brace.");
            }
            continue;
        }

        // Start of a server block
        if (line == "server {") {
            if (!inHttpBlock) {
                throw std::runtime_error("Server block outside of http block.");
            }
            if (inServerBlock) {
                throw std::runtime_error("Nested server blocks are not allowed.");
            }
            inServerBlock = true;
            currentServer = ServerConfig(); // Initialize a new server block
            continue;
        }

        // Start of a location block
        if (line.find("location ") == 0) {
            if (!inServerBlock) {
                throw std::runtime_error("Location block outside of server block.");
            }
            if (inLocationBlock) {
                throw std::runtime_error("Nested location blocks are not allowed.");
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
            throw std::runtime_error("Unexpected directive inside http block: " + line);
        } else {
            throw std::runtime_error("Unexpected directive outside of any block: " + line);
        }
    }

    // Final validations for unclosed blocks
    if (inHttpBlock) {
        throw std::runtime_error("Unclosed http block.");
    }
    if (inServerBlock) {
        throw std::runtime_error("Unclosed server block.");
    }
    if (inLocationBlock) {
        throw std::runtime_error("Unclosed location block.");
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
