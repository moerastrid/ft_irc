#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <cstdlib>
#include <string>
#include <cstring>

#define IP_ADDRESS "localhost"

class Server {
	private :
		struct addrinfo		*_ai;
		struct sockaddr_in	_sin;

		Server();								//default constructor
		void			setaddrinfo(const int port);

	public :
		~Server();								// default destructor
		Server(const Server &src);				//copy constructor
		Server &operator=(const Server &src);	// = sign operator
		Server(const int port, const std::string pass);	// constructor (PORT, pass)
		
		void	run();
};
