#pragma once

#include <cstring>
#include "Config.hpp"

class Server {
	public:
		Server(const Config& config);
		void listen();
};
