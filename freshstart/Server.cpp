#include "Server.hpp"
#include "Msg.hpp"

Server::Server() {
	Msg("Server default constructor", "DEBUG");
}

Server::~Server() {
	Msg("Server default destructor", "DEBUG");

	freeaddrinfo(_addrinfo);
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

	//_fd = socket(_addrinfo->ai_family, _addrinfo->ai_socktype, _addrinfo->ai_protocol);
	//std::cout << "fd : " << _fd << std::endl;


	(void)pass;
}


void	Server::setaddrinfo(const int port) {
	// figure out the name of this computer
	gethostname(_hostname, MAXHOSTNAMELEN);
	std::cout << _hostname << std::endl;

	// set what we want our socket to be like using "hints"
	struct addrinfo hints;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// get addr info using hings as a guide 
	int rv = getaddrinfo(_hostname, std::to_string(port).c_str(), &hints, &_addrinfo);
	if (rv != 0) {
		Msg(gai_strerror(rv), "ERROR");
		exit(EXIT_FAILURE);
	}

	_sockin.sin_family = _addrinfo->ai_family;
	_sockin.sin_port = port;
	_sockin.sin_addr.s_addr = htons(port);

	rv = inet_pton(_addrinfo->ai_family, _hostname, &(_sockin.sin_addr));
	std::cout << "rv : " << rv << std::endl;
	char* ip_str = inet_ntoa(_sockin.sin_addr);
   	std::cout << "Your IP address is: " << ip_str << std::endl;

	// inet_ntoa()
}


// void	Server::setaddrinfo(const int port) {
	
	// struct addrinfo hints;
	// memset(&hints, 0, sizeof(hints));
	// memset(&ipv4, 0, sizeof(ipv4));
	// memset(&_ai, 0, sizeof(_ai));
	// memset(&_sin, 0, sizeof(_sin));
	// hints.ai_family = AF_INET;
	// hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE;
	// int a = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &_ai);
	// if (a != 0) {
	// 	Msg(gai_strerror(a), "ERROR");
	// 	exit(EXIT_FAILURE);
	// }

	// _sin.sin_family = _ai->ai_family;
	// _sin.sin_port = port;
	// _sin.sin_addr.s_addr = htons(port);

	// // switch (inet_pton(_ai->ai_family, "127.0.0.1", &(_sin.sin_addr))) {
	// // 	case 1:
	// // 		Msg("inet_pton success", "DEBUG");
	// // 		break ;
	// // 	case 0:
	// // 		Msg("inet_pton 0?! (please use 127.0.0.1 instead of localhost)", "DEBUG");
	// // 		break ;
	// // 	case -1:
	// // 		Msg("inet_pton error", "DEBUG");
	// // 		perror("inet_pton");
	// // 		exit(EXIT_FAILURE);
	// // }

	// inet_ntop(_ai->ai_family, &(_sin.sin_addr), ipv4, INET_ADDRSTRLEN);
	// std::cout << "ipv4 : " << ipv4 << std::endl;
// }