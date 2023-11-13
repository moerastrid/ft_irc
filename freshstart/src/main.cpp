#include "ircserver.hpp"
#include "Msg.hpp"
#include "Server.hpp"


int main(int argc, char **argv) {
	int			port = -1;
	env			e;

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Server ircServer(port, argv[2]);
	std::cout << "Successfully booted the REAL TALK IRC " << ircServer << " \\^.^/" << std::endl;
	
	e.server_address = ircServer.getIP();
	e.server_password = argv[2];

	while (1) {
		ircServer.run(e);
	}
	std::cout << "Successfully ended the REAL TALK IRC " << ircServer << " \\^.^/" << std::endl;
	return (EXIT_SUCCESS);
}
