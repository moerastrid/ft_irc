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
	// std::cout << "Welcome to our IRC " << ircServer << " \\^.^/" << std::endl;
	while (1) {
		ircServer.run();
	}
	return (EXIT_SUCCESS);
}