#include "connection.hpp"
#include <errno.h>

// connection::connection() {
// 	this->fd = -1;
// }

connection::~connection() {}

connection::connection(const connection& src) {
	*this = src;
}

connection& connection::operator=(const connection& src) {
	this->_fd = src._fd;
	return *this;
}

connection::connection(int	sockfd) {
	struct sockaddr_in	new_addr;
	int					new_len = sizeof(new_addr);

	_fd = accept4(sockfd, (struct sockaddr *)&new_addr, (socklen_t*)&new_len, SOCK_NONBLOCK);
	perror("accept()");
	// _fd = accept4(sockfd, (struct sockaddr *)&addr, &len, SOCK_NONBLOCK);
}

int	connection::get_fd() {
	return (_fd);
}