#pragma once

namespace utils {
	bool setNonBlocking(int fd);
	int createPassiveSocket(int port, int backlog, bool isNonBlocking);
}