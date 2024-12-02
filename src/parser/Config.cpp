#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>


// Function to parse the configuration text
Http ConfigParser::parse(const string& configText) {
    Http httpConfig;
    stringstream ss(configText);
    string line;
    string currentBlock;
    string currentRoute;

    while (getline(ss, line)) {
        line = trim(line);
        if (line.empty() || isComment(line)) {
            continue;
        }

        // Check if starting a new block
        if (line.find("http {") != string::npos) {
            currentBlock = "http";
            continue;
        }
        if (line.find("server {") != string::npos) {
            currentBlock = "server";
            continue;
        }
        if (line.find("route") != string::npos) {
            regex routeRegex(R"(route\s+([^\s]+)\s*{)");
            smatch match;
            if (regex_search(line, match, routeRegex)) {
                currentRoute = match[1];
                httpConfig.servers[currentBlock].routes[currentRoute] = Route();
            }
            continue;
        }
        if (line == "}") {
            currentBlock = "";
            currentRoute = "";
            continue;
        }

        // Process directive (key-value pair)
        if (currentBlock == "server") {
            regex directiveRegex(R"((\w+)\s+([^\s;]+);)");
            smatch match;
            if (regex_search(line, match, directiveRegex)) {
                string directive = match[1];
                string value = match[2];

                // Check if it's a route or regular directive
                if (currentRoute.empty()) {
                    httpConfig.servers[currentBlock].directives[directive] = value;
                } else {
                    httpConfig.servers[currentBlock].routes[currentRoute].directives[directive] = value;
                }
            }
        }
    }

    return httpConfig;
}

void ConfigParser::printConfig(const Http& config) {
	for (const auto& serverPair : config.servers) {
		const Server& server = serverPair.second;
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
}

ConfigParser ConfigParser::load(const std::string& filePath) {
    ifstream configFile(filePath);
    stringstream buffer;
    buffer << configFile.rdbuf();
    string configText = buffer.str();

    ConfigParser parser;
    Http config = parser.parse(configText);
    parser.printConfig(config);
    return parser;
}

