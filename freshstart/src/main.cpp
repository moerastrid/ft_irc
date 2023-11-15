#include "ircserver.hpp"
#include "Msg.hpp"
#include "Server.hpp"


int main(int argc, char **argv) {
	int			port = -1;

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Server ircServer(port, argv[2]);
	std::cout << "Successfully booted the REAL TALK IRC " << ircServer << " \\^.^/" << std::endl;
	
	while (1) {
		ircServer.run();
	}
	std::cout << "Successfully ended the REAL TALK IRC " << ircServer << " \\^.^/" << std::endl;
	return (EXIT_SUCCESS);
}
