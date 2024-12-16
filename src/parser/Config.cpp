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
            if (!config.host.empty() || !utils::isValidIP(value)) {
                throw ConfigError(EINVAL, "Invalid host");
            }
            config.host = value;
        }},
        {"port", [&](const string &value) {
            if (config.port != 0) {
                throw ConfigError(EINVAL, "Invalid port");
            }
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
            if (!config.clientMaxBodySize.empty() || !utils::isValidSize(value)) {
                throw ConfigError(EINVAL, "Invalid client_max_body_size");
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
            throw ConfigError(EINVAL, "Invalid directive in server block");
        }
    }
}

void ConfigParser::parseLocation(const string &line, Location &currentLocation) {
    static const ParserMap locationParsers = {
        {"root", [&](const string &value) {
            if (!currentLocation.root.empty() || !utils::isValidFilePath(value)) {
                throw ConfigError(EINVAL, "Invalid root");
            }
            currentLocation.root = value;
        }},
        {"index", [&](const string &value) {
            string fullPath = currentLocation.root + "/" + value;
            if (!currentLocation.index.empty() || !utils::isValidFilePath(fullPath)) {
                throw ConfigError(EINVAL, "Invalid index");
            }
            currentLocation.index = value;
        }},
        {"autoindex", [&](const string &value) {
            if (!currentLocation.autoIndex.empty()) {
                throw ConfigError(EINVAL, "Invalid autoindex");
            }
            currentLocation.autoIndex = value;
            currentLocation.isAutoIndex = utils::parseBool(value);
        }},
        {"methods", [&](const string &value) {
            if (!currentLocation.methods.empty()) {
                throw ConfigError(EINVAL, "Invalid methods");
            }
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
            if (!currentLocation.cgiExtension.empty()) {
                throw ConfigError(EINVAL, "Invalid cgi_extension");
            }
            currentLocation.cgiExtension = value;
        }},
        {"upload_dir", [&](const string &value) {
            if (!currentLocation.uploadDir.empty() || !utils::isValidFilePath(value)) {
                throw ConfigError(EINVAL, "Invalid upload_dir");
            }
            currentLocation.uploadDir = value;
        }},
        {"return", [&](const string &value) {
            if (!currentLocation.returnUrl.empty()) {
                throw ConfigError(EINVAL, "Invalid return");
            }
            istringstream iss(value);
            vector<string> returnParts;
            string part;
            while (iss >> part) {
                returnParts.push_back(part);
            }
            if (returnParts.size() != 2 || !utils::isValidURL(returnParts[1])) {
                throw ConfigError(EINVAL, "Invalid return");
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
        } else {
            throw ConfigError(EINVAL, "Invalid directive in location block");
        }
    }
}

void ConfigParser::parseConfig(const string &filename, Config& config) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw ConfigError(ENOENT, "Invalid file path");
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
                throw ConfigError(EINVAL, "Nested http block");
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
                throw ConfigError(EINVAL, "Invalid block closure");
            }
            continue;
        }

        // Start of a server block
        if (line == "server {") {
            if (!inHttpBlock) {
                throw ConfigError(EINVAL, "Server block outside of http block");
            }
            if (inServerBlock) {
                throw ConfigError(EINVAL, "Nested server block");
            }
            inServerBlock = true;
            currentServer = ServerConfig(); // Initialize a new server block
            continue;
        }

        // Start of a location block
        if (line.find("location ") == 0) {
            if (!inServerBlock) {
                throw ConfigError(EINVAL, "Location block outside of server block");
            }
            if (inLocationBlock) {
                throw ConfigError(EINVAL, "Nested location block");
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
            throw ConfigError(EINVAL, "Invalid directive outside of recognized blocks");
        } else {
            throw ConfigError(EINVAL, "Invalid directive outside of recognized blocks");
        }
    }

    // Final validations for unclosed blocks
    if (inHttpBlock) {
        throw ConfigError(EINVAL, "Unclosed http block");
    }
    if (inServerBlock) {
        throw ConfigError(EINVAL, "Unclosed server block");
    }
    if (inLocationBlock) {
        throw ConfigError(EINVAL, "Unclosed location block");
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
    try {
        Config config;
        parseConfig(filePath, config);
        printConfig(config);
    } catch (const ConfigError& e) {
        cout << "Error: " << e.code() << " " << e.what() << endl;
        
    }
}
