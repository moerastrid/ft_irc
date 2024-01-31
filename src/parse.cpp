/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 14:56:26 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 14:56:27 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"

static bool	isvalidpass(const std::string pass) {
	if (pass.empty())
		return (false);
	for (int i(0); pass[i]; i++) {
		if (isprint(pass[i]) == false)
			return (false);
	}
	return (true);
}

static int get_port(std::string input) {
	const std::string	WHITESPACE = " \n\r\t\f\v";
	int port = -1;
	int	place = 0;
	std::string	first;
	std::string	second;

	if (input.empty())
		return (-1);
	place = input.find_first_not_of(WHITESPACE);
	if (place == -1)
		return (-1);
	first = &input[place];
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
		Msg("invalid password, please enter (printable) characters.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		return (-1);
	}
	port = get_port(argv[1]);
	if (port < 0 || port > 65535) {
		Msg("invalid port, please enter digits bewteen 0 and 65535.\nIRCserver usage:\t./ircserver <port> <password>", "ERROR");
		return(-1);
	}
	return (port);
}