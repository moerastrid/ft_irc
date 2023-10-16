#include "ft_irc.hpp"
#include "message.hpp"
#include "server.hpp"

int main(int argc, char **argv) {
	int port = 0;

	port = parse(argc, argv);
	if (port < 0)
		exit(EXIT_FAILURE);
	server ircServer(port, argv[2]);
	while (1) {
		ircServer.run();
	}

	// message("test", "ERROR");
	// message("test", "INFO");
	// message("test", "DEBUG");
	// message("test", "WARNING");
	return (0);
}
