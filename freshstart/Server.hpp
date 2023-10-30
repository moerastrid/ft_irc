#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <poll.h>

class Server {
	private :
		char				_hostname[MAXHOSTNAMELEN];
		struct addrinfo		*_addrinfo;
		struct sockaddr_in	_sockin;
		std::string			_ip;
		struct pollfd		_sockfd;
		int					_port;
		std::string			_pass;

		//temp
		struct sockaddr_in	_their_addr; /*this will be client*/

		Server();								//default constructor

		void			setAddrInfo(const int port);
		void			setHostInfo();
		void			setUp();

	public :
		~Server();								// default destructor
		Server(const Server &src);				//copy constructor
		Server &operator=(const Server &src);	// = sign operator
		Server(const int port, const std::string pass);	// constructor (PORT, pass)
		
		void				run();

		const std::string	getIP() const;
		const std::string	getHostname() const;
		int					getPort() const;

};

std::ostream& operator<<(std::ostream& os, const Server& server);
