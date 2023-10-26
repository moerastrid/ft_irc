#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <deque>
#include "message.hpp"
#include "ft_irc.hpp"
#include "connection.hpp"


class server {
	private :
		const int			_opt = 1;
		int					_port = -1;
		int					_sockfd = -1;
		int					_timeout = -1;
		std::string			_password;
		sockaddr_in			_address;
		int					_epfd;
		struct epoll_event	_event;
		struct epoll_event	_events[MAX_CONN];
		server();								//default constructor
		std::deque<connection>	_cons;

	public :
		~server();								// default destructor
		server(const server &src);				//copy constructor
		server &operator=(const server &src);	// = sign operator
		server(const unsigned int port, const std::string password);		// constructor (PORT)
		
		void	run();
};
