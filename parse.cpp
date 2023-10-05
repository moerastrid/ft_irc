#include "ft_irc.hpp"
#include "message.hpp"

int     parse(int argc, char **argv) {
	(void)argc;
	(void)argv;
	for (int i = 0; argv[i]; i++) {
		message(argv[i], "INFO");
	}
	return (0);
}