#include <iostream>
#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		Configuration config = Configuration::load(argv[1]);
		Server server(config);
		server.listen();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
