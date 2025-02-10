#pragma once

#include "Config.hpp"
#include "Server.hpp"
#include <functional>

namespace utils {
	bool setNonBlocking(int fd);
	int createPassiveSocket(const char* host, int port, int backlog, bool isNonBlocking);
}
