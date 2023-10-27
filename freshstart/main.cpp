#include "ircserver.hpp"
#include "Msg.hpp"

int main(int argc, char **argv) {
	int			port = -1;

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}
	
// 	Server ircServer(port, argv[2]);
// 	while (1) {
// 		ircServer.run();
// 	}
	return (EXIT_SUCCESS);
}