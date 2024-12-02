#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


void Config::parseLine(const std::vector<std::string>& tokens, ConfigData& config, std::string& currentRoute) {
    if (tokens.empty()) {
        return;
    }
    if (tokens[0] == "route") {
        if (tokens.size() < 2) {
            throw std::runtime_error("Route directive requires a path");
        }
        currentRoute = tokens[1];
        config.routes[currentRoute] = Config();
    } else if (tokens[0] == "end") {
        currentRoute = "";
    } else {
        if (currentRoute.empty()) {
            if (tokens.size() < 2) {
                throw std::runtime_error("Directive requires a value");
            }
            config.directives[tokens[0]] = tokens[1];
        } else {
            parseLine(tokens, config.routes[currentRoute].data, currentRoute);
        }
    }
}

std::vector<std::string> Config::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void Config::printConfig(const ConfigData& config, int indent) const {
        std::string indentStr(indent, ' ');
        for (const auto& [key, value] : config.directives) {
            std::cout << indentStr << key << ": " << value << std::endl;
        }
        for (const auto& [route, routeConfig] : config.routes) {
            std::cout << indentStr << "route " << route << " {" << std::endl;
            printConfig(routeConfig.data, indent + 4);
            std::cout << indentStr << "}" << std::endl;
        }
    }

void Config::print() const {
    printConfig(data);
}

Config Config::load(const std::string& filePath) {
    Config config;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    std::string line;
    std::string currentRoute = "";
    while (std::getline(file, line)) {
        auto tokens = config.tokenize(line);
        config.parseLine(tokens, config.data, currentRoute);
    }
    return config;
}


