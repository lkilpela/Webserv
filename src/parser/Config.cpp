#include "Config.hpp"
#include <fstream> // std::ifstream, std::getline
#include <iostream> // std::cout, std::endl
#include <sstream> // std::istringstream
#include <regex> // std::regex, std::regex_match
#include <string> // std::string

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

void validateMethods(const vector<string> &methods) {
    vector<string> validMethods = {"GET", "POST", "PUT", "DELETE"};
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

// Function to parse global configuration lines
void ConfigParser::parseGlobal(const string &line, ServerConfig &config) {
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
            int errorCode;
            string filePath;
            iss >> errorCode >> filePath;
            if (!isValidFilePath(filePath)) {
                throw std::invalid_argument("Invalid file path: " + value);
            }
            config.errorPages[errorCode] = filePath; // Store in map
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
        } else {
            throw std::invalid_argument("Unknown configuration key: " + key);
        }
    }
}

void ConfigParser::parseLocation(const string &line, Location &currentLocation) {
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
        line = trim(removeComments(line));
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
