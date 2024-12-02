#pragma once

#include <cstring>
#include "Configuration.hpp"

class Server {
	public:
		Server(const Configuration& config);
		void listen();
};
