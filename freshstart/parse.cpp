#include "Msg.hpp"
#include <iostream>
#include <string>
#include <algorithm>

static bool	isvalidpass(const std::string pass) {
	if (pass.empty())
		return (false);
	for (int i(0); pass[i]; i++) {
		if (isalnum(pass[i]) == false)
			return (false);
	}
	return (true);
}

static int get_port(std::string input) {
	const std::string	WHITESPACE = " \n\r\t\f\v";
	int port = -1;
	std::string	first;
	std::string	second;

	first = &input[input.find_first_not_of(WHITESPACE)];
	reverse(first.begin(), first.end());
	second = &first[first.find_first_not_of(WHITESPACE)];
	reverse(second.begin(), second.end());
	for (int i = 0; second[i]; i++) {
		if (isdigit(second[i]) == false)
			return(-1);
	}
	port = atoi(second.c_str());
	return (port);
}

int	parse(int argc, char **argv) {
	int port = -1;

	if (argc != 3) {
		Msg("wrong amount of arguments.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		return (-1);
	} 
	if (isvalidpass(argv[2]) == false) {
		Msg("invalid password, please enter (alphanumerical) chars.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		return (-1);
	}
	port = get_port(argv[1]);
	if (port < 0) {
		Msg("invalid port, please enter digits bewteen 0 and 65535.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
	}
	return (port);
}