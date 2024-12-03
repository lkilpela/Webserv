#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

// Define the structure to hold the configuration
struct Http {
	map<string, Server> servers;
};

struct Server {
	map<string, string> directives;
	map<string, Route> routes;
};

struct Route {
	map<string, string> directives;
};

class ConfigParser {
private:
    // Helper functions
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
    }

    bool isComment(const string& line) {
        return line.find("#") == 0;
    }


public:

	ConfigParser() = default;

    // Function to parse the configuration text
    Http parse(const string& configText);
	static ConfigParser load(const std::string& filePath);

	void printConfig(const Http& config);
};
