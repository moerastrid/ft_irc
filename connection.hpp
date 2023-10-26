#pragma once
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <deque>

class connection {
	private:
		// int	_fd = -1;

	public:
		int	_fd = -1;
		connection();
		connection(int	sockfd);
		~connection();
		connection(const connection& other);
		connection& operator=(const connection& other);

		int	get_fd();
};
