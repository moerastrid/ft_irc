#include "ft_irc.hpp"
#include "message.hpp"
#include <cstring>

static int	get_port(const	std::string	strp) {
	const std::string WHITESPACE = " \n\r\t\f\v";
	int port = -1;
	std::string	trimmed;

	int i = 0;
	if (strp.empty())
		return (port);

	trimmed = &strp[strp.find_first_not_of(WHITESPACE)];
	// message(strp.find_first_not_of(WHITESPACE), "DEBUG");
	trimmed[trimmed.find_last_not_of(WHITESPACE) + 1] = 0;
	message("strp", "DEBUG");
	message(strp, "DEBUG");
	message("trimmed.", "DEBUG");
	message(trimmed, "DEBUG");
	port = atoi(trimmed.c_str());
	std::cout << "port: " << port << std::endl;
	return (port);

	// while (isspace(strp[i]) != false)
	// 	i++;
	// if (strlen(&strp[i]) >= 6)
	// 	return (-1);
	// for (int j(0); strp[i + j]; j++) {
	// 	if (isdigit(strp[i + j] == false))
	// 		return (-1);
	// }
	// port = atoi(&strp[i]);
	// message(&strp[i], "DEBUG");

	// message("end of get_port()", "DEBUG");
	// std::cout << port << std::endl;
	// if (port < 0 || port > 65535)
	// 	port = -1;
	// return (port);
}

static bool	isvalidpass(std::string strpass) {
	if (strpass.empty())
		return (false);
	for (int i(0); strpass[i]; i++) {
		if (isprint(strpass[i]) == false)
			return (false);
	}
	return (true);
}

int     parse(int argc, char **argv) {
	if (argc != 3) {
		message("wrong amount of arguments.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		exit (EXIT_FAILURE);
	}

	if (isvalidpass(argv[2]) == false) {
		message("invalid password, please enter (printable) chars.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		exit (EXIT_FAILURE);
	}

	int port = get_port(argv[1]);
	if (port == -1) {
		message("invalid port, please enter digits bewteen 0 and 65535.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
	 	exit (EXIT_FAILURE);
	}
	for (int i = 0; argv[i]; i++) {
		message(argv[i], "INFO");
	}
	return (port);
}
