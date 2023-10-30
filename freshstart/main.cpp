#include "ircserver.hpp"
#include "Msg.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
	int			port = -1;

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}
	std::cout << "PORT : " << port << std::endl;
	
	Server ircServer(port, argv[2]);
// 	while (1) {
// 		ircServer.run();
// 	}
	return (EXIT_SUCCESS);
}