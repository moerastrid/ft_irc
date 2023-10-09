#include "ft_irc.hpp"
#include "message.hpp"
#include "server.hpp"

int main(int argc, char **argv) {
	int port = 8080;
	if (parse(argc, argv) != 0)
		return (1);
	server ircServer(port);
	ircServer.run();
	(void)argc;
	(void)argv;

	// message("test", "ERROR");
	// message("test", "INFO");
	// message("test", "DEBUG");
	// message("test", "WARNING");
	return (0);
}
