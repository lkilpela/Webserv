#include "utils/common.hpp"
#include "Config.hpp"
#include "Error.hpp"
//#include "Server.hpp"
#include <sstream> // std::istringstream

// Define namespaces
using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::cout;
using std::endl;
namespace fs = std::filesystem;

void ConfigParser::parseHttpBlock(ifstream &file, Config &config) {
	utils::parseBlock(file, "http", [&](const string &line) {
		if (line == "server {") {
			ServerConfig server;
			parseServerBlock(file, server);
			config.servers.push_back(server);
		} else {
			throw ConfigError(EINVAL, "Invalid directive in http block: " + line);
		}
	});
}

void ConfigParser::parseServerBlock(ifstream &file, ServerConfig &server) {
	utils::parseBlock(file, "server", [&](const string &line) {
		if (line.find("location ") == 0) {
		Location location;
		auto [key, path] = utils::splitKeyValue(line);
		location.path = path;
		parseLocationBlock(file, location);
		server.locations.push_back(location);
		} else {
			parseGlobal(line, server);
		}
	});
}

void ConfigParser::parseLocationBlock(ifstream &file, Location &location) {
	utils::parseBlock(file, "location", [&](const string &line) {
		parseLocation(line, location);
	});
}

// Function to parse global configuration lines
void ConfigParser::parseGlobal(const string &line, ServerConfig &server) {
    static const ParserMap globalParsers = {
        {"host", [&](const string &value) {
            if (!server.host.empty() || !utils::isValidIP(value)) {
                throw ConfigError(EINVAL, "Invalid host");
            }
            server.host = value;
        }},
        {"port", [&](const string &value) {
            if (server.port != 0) {
                throw ConfigError(EINVAL, "Invalid port");
            }
            server.port = utils::parsePort(value);
        }},
        {"server_name", [&](const string &value) {
            if (!server.serverName.empty()) {
                throw ConfigError(EINVAL, "Invalid server_name");
            }
            server.serverName = value;
        }},
        {"error_page", [&](const string &value) {
            istringstream iss(value);
            std::string code, path;
            iss >> code >> path;
			fullPath = getConfigPath(path);
            utils::validateErrorPage(code, fullPath);
            server.errorPages[std::stoi(code)] = fullPath; // Store in map
        }},
        {"client_max_body_size", [&](const string &value) {
			if (!server.clientMaxBodySizeStr.empty() || !utils::isValidSize(value)) {
                throw ConfigError(EINVAL, "Invalid client_max_body_size");
            }
			server.clientMaxBodySize = utils::convertSizeToBytes(value);
        }}
    };

	utils::parseKeyValue(line, globalParsers);
}

// filePath as argv[1] = config/webserv.conf
// value: /static/
// parent_path: /Users/username/Webserv/config/
// return: /Users/username/Webserv/config/static/
fs::path ConfigParser::getConfigPath(const string &value) const {
	if (filePath.empty() || value.empty()) {
		throw ConfigError(EINVAL, "Invalid path");
	}
	fs::path fullPath = utils::sanitizePath(filePath.parent_path(), value);
	return fullPath;

}

void ConfigParser::parseLocation(const string &line, Location &currentLocation) {
	static const ParserMap locationParsers = {
		{"root", [&](const string &value) {
			fullPath = getConfigPath(value);
			if (!currentLocation.root.empty() || !utils::isValidFilePath(fullPath)) {
				throw ConfigError(EINVAL, "Invalid root");
			}
			currentLocation.root = fullPath;
		}},
		{"index", [&](const string &value) {
			fullPath = currentLocation.root/value;
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
			currentLocation.cgiExtension.push_back(value);
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

	utils::parseKeyValue(line, locationParsers);
}

void ConfigParser::parseConfig(const string &filename, Config& config) {
	ifstream file(filename);
	if (!file.is_open()) {
		throw ConfigError(ENOENT, "Unable to open configuration file.");
	}

	string line;
	while (std::getline(file, line)) {
		line = utils::trim(utils::removeComments(line));
		if (line.empty()) continue;

		if (line == "http {") {
			parseHttpBlock(file, config);
		} else {
			throw ConfigError(EINVAL, "Invalid directive outside of blocks: " + line);
		}
	}
}


// Function to load the configuration
Config ConfigParser::load() {
	Config config;
	parseConfig(filePath, config);
	//utils::printConfig(config); // For testing
	return config;
}