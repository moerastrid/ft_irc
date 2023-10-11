#include "ft_irc.hpp"
#include "message.hpp"
// #include "string"

int     parse(int argc, char **argv) {
	int port = -1;
	if (argc != 3) {
		message("wrong amount of arguments.\nusage: ./ircserver <port> <password>", "ERROR");
		exit (EXIT_FAILURE);
	}
	port = atoi(argv[1]);
	message(std::to_string(port), "DEBUG");
	for (int i = 0; argv[i]; i++) {
		message(argv[i], "INFO");
	}
	return (port);
}