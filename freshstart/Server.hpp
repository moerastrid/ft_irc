#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

// #ifndef MAXHOSTNAMELEN
// #define MAXHOSTNAMELEN 64
// #endif

class Server {
	private :
		char				_hostname[MAXHOSTNAMELEN];
		struct addrinfo		*_addrinfo;
		struct sockaddr_in	_sockin;
		char				ipv4[INET_ADDRSTRLEN];
		int					_fd;

		Server();								//default constructor
		void			setaddrinfo(const int port);

	public :
		~Server();								// default destructor
		Server(const Server &src);				//copy constructor
		Server &operator=(const Server &src);	// = sign operator
		Server(const int port, const std::string pass);	// constructor (PORT, pass)
		
		void	run();
};
