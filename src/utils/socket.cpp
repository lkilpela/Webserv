#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include "utils/socket.hpp"

namespace utils {
	bool setNonBlocking(int fd) {
		int flags = fcntl(fd, F_GETFL, 0);

		if (flags == -1) {
			return false;
		}

		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			return false;
		}

		return true;
	}

	int createPassiveSocket(const char* host, int port, int backlog, bool isNonBlocking) {
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd == -1) {
			throw std::runtime_error("Failed to create socket");
		}

		if (isNonBlocking && setNonBlocking(fd) == false) {
			throw std::runtime_error("Failed to set non-blocking");
		}

        sockaddr_in address {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(host);
        address.sin_port = htons(port);

        if (::bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
	        throw std::runtime_error("Failed to bind socket on port " + std::to_string(port));
		}

        if (::listen(fd, backlog) < 0) {
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
		}

		return fd;
	}
}
