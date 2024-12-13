#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include <exception>

//#include "Server.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		ConfigParser parser;
		parser.load(argv[1]);
		
		//Server server(config);
		///server.listen();
	} catch (const ConfigException& e) {
		std::cerr << "Error: " << e.what() << " (code: " << static_cast<int>(e.code()) << ")" << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
