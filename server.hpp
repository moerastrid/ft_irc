#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include "message.hpp"
#include "ft_irc.hpp"


class server {
	private :
		int				_port = -1;
		int				_server_fd = 0;
		int				_connection = 0;
		std::string		_password;
		sockaddr_in		_address;
		server();								//default constructor

	public :
		~server();								// default destructor
		server(const server &src);				//copy constructor
		server &operator=(const server &src);	// = sign operator
		server(const unsigned int port, const std::string password);		// constructor (PORT)
		
		void	run();
};
