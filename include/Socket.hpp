#ifndef SOCKET_HPP
#define SOCKET_HPP
#define PORT 8080

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>

class Socket{

	public:
		Socket(){}
		~Socket(){}
		std::vector<int> getPorts(){return ports;}
	private:
		std::vector<int> ports = {8080, 8081};
};

#endif