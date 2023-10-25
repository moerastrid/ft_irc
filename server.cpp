#include "server.hpp"
#include <cstring>

server::server() {}

server::~server() {
	close(_server_fd);
	// close epoll when done
	if (close(_epoll_fd) < 0) {
		message("close(epoll_fd) ", "ERROR");
		exit(EXIT_FAILURE);
	}
}

server::server(const server &src) {
	*this = src;
}

server &server::operator=(const server &src) {
	this->_port = src._port;
	this->_server_fd = src._server_fd;
	this->_connection = src._connection;
	return (*this);
}

server::server(const unsigned int port, std::string password) : _port(port), _password(password) {
	this->_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (this->_server_fd < 0) {
		message("cannot create socket", "ERROR");
		exit(EXIT_FAILURE);
	} else {
		message("socket created", "INFO");
	}

	// nog uitzoeken wat dit precies is
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&_opt, sizeof(_opt)) == -1) {
		message("cannot set socket opt", "ERROR");
		close(this->_server_fd);
		exit(EXIT_FAILURE);
	}

	// bind socket
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(this->_port);
	this->_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->_server_fd, ((const struct sockaddr *)&this->_address), sizeof(this->_address)) < 0) {
		message("Cannot bind socket", "ERROR");
		close(this->_server_fd);
		exit(EXIT_FAILURE) ;
	} else
		message("socket bind success", "INFO");

	// wait for _connection
	if (listen(this->_server_fd, MAX_CONNECT) < 0) {
		message("Cannot hear socket", "ERROR");
		close(this->_server_fd);
		exit(EXIT_FAILURE) ;
	} else
		message("socket loud and clear over", "INFO");


	// eerst een epoll instance
	_epoll_fd = epoll_create1(0);

	if (_epoll_fd < 0) {
		message("epoll_fd", "ERROR");
		exit(EXIT_FAILURE);
	} else {
		message("epoll fd created", "INFO");
	}

	// add file descriptors to be monitored & specify what to monitor (EPOLLET specifies non-blocking)

	_ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
	_ev.data.fd = this->_server_fd;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, this->_server_fd, &_ev) < 0) {
		message("epoll_ctl", "ERROR");
		close(_epoll_fd);
		exit(EXIT_FAILURE);
	}
}

// static void	new_connection(int fd) {
// 	struct sockaddr_in	new_addr;
// 	int					_new_conn;
// 	int					new_len = sizeof(new_addr);

// 	while (1) {
// 		int	_new_conn = accept(_server_fd, (const struct sockaddr*)&new_addr, &new_len);
// 		if (_new_conn < 0) {
// 			// we have processed all incoming connections
// 			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
//                 break;
//             }
//             else {
//                 message("cannot accept socket", "WARNING");
//                 break;
//             }
// 		} else {
// 			message("socket accepted", "INFO");
// 		}

// 		// Allow epoll to monitor new connection

// 		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _new_conn, &_ev) == -1) {
// 	 		message("epoll_ctl: conn_sock", "WARNING");
// 	 		break;
// 	 	} else {
// 	 		message("connection added", "INFO");
// 	 	}
// 	}
// }

void server::run() {
	message("start while loop", "DEBUG");
	// RETURN VALUE		epoll_wait():
    //    When successful, epoll_wait() returns the number of file descriptors ready for  the
    //    requested  I/O,  or  zero  if  no file descriptor became ready during the requested
    //    timeout milliseconds.  When an error occurs, epoll_wait() returns -1 and  errno  is
    //    set appropriately
	int timeout = -1;
	// int timeout = 2000; // time in ms. timeout=0 makes epoll return immediately even if no events are ready
	int	num_fds = epoll_wait(_epoll_fd, _all_events, MAX_CONNECT, timeout);

	if (num_fds < 0) {
		message("epoll_wait", "ERROR");
		exit(EXIT_FAILURE);
	} else if (num_fds == 0) {
		message("no connect/communication made or none of the fd's are ready", "INFO");
	} else {
		std::cout << "fd: " << num_fds << std::endl;
		// Iterate over all the file descriptors ready for the requested I/O.
		for (int i = 0; i < num_fds; i++) {
			int fd = _all_events[i].data.fd;

			// new connection :
			if (fd == _server_fd) {
				// new_connection(fd);
				struct sockaddr_in	new_addr;
				int					_new_conn;
				int					new_len = sizeof(new_addr);

				// while (1) {
					_new_conn = accept(_server_fd, (struct sockaddr*)&new_addr, (socklen_t*)&new_len);
					if (_new_conn < 0) {
						// we have processed all incoming connections
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							break;
						}
						else {
							message("cannot accept socket", "WARNING");
							break;
						}
					} else {
						message("socket accepted", "INFO");
					}

					// Allow epoll to monitor new connection
					if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _new_conn, &_ev) == -1) {
						message("epoll_ctl: conn_sock", "WARNING");
						break;
					} else {
						message("connection added", "INFO");
						std::string welcom = ":localhost 001 tnuyten :Welcome to the Internet Relay Network Neus!tnuyten@localhost\n";
						send(_new_conn, welcom.c_str(), welcom.size(), 0);
					}
				// }
			}

			// excisting connection with a problem
			else if (_all_events[i].events & EPOLLERR) {
				message("EPOLLERR received", "WARNING");
				// close connection with client
            	close(fd);
       		}

			// excisting connection sending or receiving data
			else {
				message("fd != server_fd", "INFO");
				std::cout << "HELLO THIS IS HAPPENING!!!" << std::endl;
				char buf[100000];
				bzero(buf, sizeof(buf));
					int n = read(_all_events[i].data.fd, buf,
						 sizeof(buf));
					std::cout << "buf : " << std::endl;
					if (n <= 0 /* || errno == EAGAIN */ ) {
						break;
					} else {
						printf("[+] data: %s\n", buf);
						write(_all_events[i].data.fd, buf,
						      strlen(buf));
					}
			}
		}
	}


	// socklen_t len = sizeof(this->_address);

	// begin loop? voor meerdere _connections
	
		// _connection = accept(_server_fd, (struct sockaddr *)&_address, &len);
		// if (_connection < 0) {
		// 	// std::cerr << "Cannot accept socket" << std::endl;
		// 		(void)_connection;
		// 	} else {
		// 	std::cout << "socket " << _server_fd << " accepted" << std::endl;

		// 	// receive info
		// 	char buf[40000];
		// 	long valread = read(_connection, buf, 40000);
		// 	std::cout << buf << std::endl;
		// 	std::cout << "valread : " << valread << std::endl;

		// 	// send message
		// 	std::string response = "this is FT_IRC over \n";
			
		// 	send(_connection, response.c_str(), response.size(), 0);
		// 	std::cout << "hello message sent" << std::endl;
		// 	}
		// // pas helemaal als je klaar bent:
		// close(_connection);
}

