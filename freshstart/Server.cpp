#include "Server.hpp"
#include "Msg.hpp"

Server::Server() {
	Msg("Server default constructor", "DEBUG");
}

Server::~Server() {
	Msg("Server default destructor", "DEBUG");

	freeaddrinfo(_ai);
}

Server::Server(const Server &src) {
	Msg("Server copy contructor", "DEBUG");
	*this = src;
}

Server &Server::operator=(const Server &src) {
	Msg("Server assignment operator", "DEBUG");
	(void)src;
	return (*this);
}

Server::Server(const int port, const std::string pass) {
	Msg("Server constructor (PORT, pass)", "DEBUG");

	Server::setaddrinfo(port);
	
	(void)pass;
}





void	Server::setaddrinfo(const int port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	memset(&_ai, 0, sizeof(_ai));
	memset(&_sin, 0, sizeof(_sin));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int a = getaddrinfo(IP_ADDRESS, std::to_string(port).c_str(), &hints, &_ai);
	if (a != 0) {
		Msg(gai_strerror(a), "ERROR");
		exit(EXIT_FAILURE);
	}
	_sin.sin_family = _ai->ai_family;
	_sin.sin_port = port;
}