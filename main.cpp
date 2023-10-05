#include "ft_irc.hpp"
#include "message.hpp"

int main(int argc, char **argv) {
	sayhi();
	if (parse(argc, argv) != 0)
		return (1);
	// server();
	(void)argc;
	(void)argv;

	return (0);
}
