#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

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
    for (const auto& routePair : server.routes) {
        const Route& route = routePair.second;
        cout << "  Route: " << routePair.first << "\n";
        for (const auto& routeDirective : route.directives) {
            cout << "    " << routeDirective.first << ": " << routeDirective.second << endl;
        }
    }
}

ConfigParser ConfigParser::load(const std::string& filePath) {
    ifstream configFile(filePath);
    stringstream buffer;
    buffer << configFile.rdbuf();
    string configText = buffer.str();

    ConfigParser parser;
    Server config = parser.parse(configText);
    parser.printConfig(config);
    return parser;
}

