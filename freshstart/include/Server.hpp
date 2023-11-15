#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <poll.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#define BUFSIZE 512

class Server {
	private :
		Server();										//default constructor

        const string            _name = "REAL TALK IRC";

		struct sockaddr_in		_sockin;
		struct pollfd			_sockfd;

		int						_port;
		string					_pass;
		vector<struct pollfd>	_pollFds;

		void	setUp();
		int		setPoll();
		void	addConnection();
		void	closeConnection(const int i);
		string	receive(int fd);

	public :
		~Server();										// default destructor
		Server(const Server &src);						//copy constructor
		Server &operator=(const Server &src);			// = sign operator
		Server(const int port, const string pass);		// constructor (PORT, pass)
		
		void				run();
		const string		getIP() const;
		int					getPort() const;
        const string        getName() const;
};

std::ostream& operator<<(std::ostream& os, const Server& server);
