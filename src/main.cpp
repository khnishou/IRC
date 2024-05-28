
#include "../include/Server.hpp"
#include "../include/Users.hpp"
#include "../include/Channel.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv <port number> <password>" << std::endl;
		exit(EXIT_SUCCESS);
	}
	std::string pswd = argv[2];
	std::string p = argv[1];
	if (!isUint(p)) {
		std::cerr << "Error: Invalid port!" << std::endl;
		exit(EXIT_SUCCESS);
	}
	int port = std::atoi(argv[1]);
	if (port < 6660 || (port > 6669 && port != 7000)) {
		std::cerr << "Error: Invalid port!" << std::endl;
		exit(EXIT_SUCCESS);
	}
	Server serv(port, pswd);

	while (serv.getState() == START) {
		serv.init();
		serv.start();
	}
	exit(EXIT_SUCCESS);
}

