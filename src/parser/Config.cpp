#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

// Function to parse the configuration text
Server ConfigParser::parse(const string& configFilePath) {
    Server server;
    ifstream configFile(configFilePath);
    string line;
    string currentSection;

    while (getline(configFile, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line.find("server {") != string::npos) {
            currentSection = "server";
        } else if (line.find("location") != string::npos) {
            currentSection = "location";
            string locationPath = extractLocationPath(line);
            server.routes[locationPath] = Route();
        } else if (line.find("}") != string::npos) {
            currentSection = "";
        } else {
            if (currentSection == "server") {
                auto directive = parseDirective(line);
                server.directives[directive.first] = directive.second;
            } else if (currentSection == "location") {
                auto directive = parseDirective(line);
                server.routes.rbegin()->second.directives[directive.first] = directive.second;
            }
        }
    }

    return server;
}

void ConfigParser::printConfig(const Server& server) {
    cout << "Server block: \n";
    for (const auto& directive : server.directives) {
        cout << "  " << directive.first << ": " << directive.second << endl;
    }
    cout << "Routes:\n";
    for (const auto& routePair : server.routes) {
        const Route& route = routePair.second;
        cout << "  Location: " << routePair.first << "\n";
        for (const auto& routeDirective : route.directives) {
            cout << "    " << routeDirective.first << ": " << routeDirective.second << endl;
        }
    }
}

ConfigParser ConfigParser::load(const string& filePath) {
    ConfigParser parser;
    Server config = parser.parse(filePath);
    parser.printConfig(config);
    return parser;
}

