#pragma once

#include <cstring>

class Configuration {
	public:
		static Configuration load(const std::string& config);
};
