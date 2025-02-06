#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
//#include "Server.hpp"
#include <exception>
#include <thread>
//#include "SignalHandle.hpp"
#include "http/index.hpp"

volatile sig_atomic_t sigintReceived = 0;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		//handleSignals();
		ConfigParser parser(argv[1]);
		Config config = parser.load();
		//std::vector<Server> servers;
		//Server server(config);
		//server.listen();
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.code().message() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
