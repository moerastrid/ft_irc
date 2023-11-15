#include "ircserver.hpp"


int main(int argc, char **argv) {
	int			port;

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Server ircServer(port, argv[2]);
	std::cout << "Successfully booted the " << ircServer << " \\^.^/" << std::endl;
	
	while (1) {
		ircServer.run();
	}
	std::cout << "Successfully ended the " << ircServer << " \\^.^/" << std::endl;
	return (EXIT_SUCCESS);
}
