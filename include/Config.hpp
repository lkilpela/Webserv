#pragma once

#include <cstring>
#include <map>
#include <vector>


struct ConfigData {
    std::map<std::string, std::string> directives;
    std::map<std::string, Config> routes;
};

class Config {
private:
	ConfigData data;

public:

	std::vector<std::string> tokenize(const std::string& line);
	void parseine(const std::vector<std::string>& tokens,
					ConfigData& config, std::string& currentRoute);
	static Config load(const std::string& filename);
	void printConfig(const ConfigData& config, int indent = 0) const;
	
};
