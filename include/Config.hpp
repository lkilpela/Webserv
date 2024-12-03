#pragma once

#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::pair;

// Define the structure to hold the configuration
struct Route {
	map<string, string> directives;
};

struct Server {
	map<string, string> directives;
	map<string, Route> routes;
};

class ConfigParser {
private:
    // Helper functions
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
    }

    pair<string, string> parseDirective(const string& line) {
        size_t pos = line.find(' ');
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        return make_pair(trim(key), trim(value));
    }

    string extractLocationPath(const string& line) {
        size_t start = line.find(' ') + 1;
        size_t end = line.find(' ', start);
        return line.substr(start, end - start);
    }

public:

	ConfigParser() = default;

    // Function to parse the configuration text
    Server parse(const string& configFilePath);
	static ConfigParser load(const std::string& filePath);
	void printConfig(const Server& server);
};
