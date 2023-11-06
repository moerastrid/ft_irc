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
#include <deque>
#include <vector>

class Server {
	private :
		Server();										//default constructor
		char				_hostname[MAXHOSTNAMELEN];
		struct sockaddr_in	_sockin;
		std::string			_ip;
		struct pollfd		_sockfd;
		int					_port;
		std::string			_pass;
		std::vector<struct pollfd>	_pollFds;

		void			setAddrInfo(const int port);
		void			setHostInfo();
		void			setUp();

	public :
		~Server();										// default destructor
		Server(const Server &src);						//copy constructor
		Server &operator=(const Server &src);			// = sign operator
		Server(const int port, const std::string pass);	// constructor (PORT, pass)
		
		void				run();

		const std::string	getIP() const;
		const std::string	getHostname() const;
		int					getPort() const;

};

std::ostream& operator<<(std::ostream& os, const Server& server);
