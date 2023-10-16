#include "server.hpp"

server::server() {}

server::~server() {
	close(_server_fd);
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
	this->_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_server_fd < 0) {
		message("cannot create socket", "ERROR");
		exit(EXIT_FAILURE);
	} else {
		message("socket created", "INFO");
	}

	// bind socket
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(this->_port);
	this->_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->_server_fd, ((const sockaddr *)&this->_address), sizeof(this->_address)) < 0) {
		message("Cannot bind socket", "ERROR");
		exit(EXIT_FAILURE) ;
	} else
		message("socket bind success", "INFO");

	// wait for _connection
	if (listen(this->_server_fd, MAX_CONNECT) < 0) {
		message("Cannot hear socket", "ERROR");
		exit(EXIT_FAILURE) ;
	} else
		message("socket loud and clear over", "INFO");
}

void server::run() {
	// eerst een epoll instance
	int epoll_fd = epoll_create1(0);

	if (epoll_fd < 0) {
		message("epoll_fd", "ERROR");
		exit(EXIT_FAILURE);
	}

	// add file descriptors to be monitored & specify what to monitor (EPOLLET specifies non-blocking)
	struct epoll_event	ev;
	struct epoll_event	all_events[MAX_CONNECT];

	ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
	ev.data.fd = this->_server_fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->_server_fd, &ev) < 0) {
		message("epoll_ctl", "ERROR");
		close(epoll_fd);
		exit(EXIT_FAILURE);
	}

	/*RETURN VALUE		epoll_wait():
       When successful, epoll_wait() returns the number of file descriptors ready for  the
       requested  I/O,  or  zero  if  no file descriptor became ready during the requested
       timeout milliseconds.  When an error occurs, epoll_wait() returns -1 and  errno  is
       set appropriately*/
	int timeout = 1000; // time in ms. timeout=0 makes epoll return immediately even if no events are ready
	int	num_fds = epoll_wait(epoll_fd, all_events, MAX_CONNECT, timeout);
	if (num_fds < 0) {
		message("epoll_wait", "ERROR");
		close(epoll_fd);
		exit(EXIT_FAILURE);
	} else if (num_fds == 0) {
		message("no connect/communication made or none of the fd's are ready", "INFO");
	} else {
		// Iterate over all the file descriptors ready for the requested I/O.
		message("HELLO?", "WARNING");
		for (int i = 0; i < num_fds; i++) {
			int fd = all_events[i].data.fd;

			// new connection :
			if (fd == _server_fd) {
				message("fd == server_fd", "INFO");
				socklen_t len = sizeof(this->_address);
				_connection = accept(_server_fd, (struct sockaddr *)&_address, &len);

				if (_connection < 0) {
					/* We have processed all incoming connections. */
					message("cannot accept socket", "WARNING");
					break ;
					// if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
					// 	break;
					// }
					// else {
					// 	perror ("accept");
					// 	break;
					// }
				} else {
					message("socket accepted", "INFO");
					// make connection non-blocking
					fcntl(_connection, F_SETFL, O_NONBLOCK);

					// Allow epoll to monitor new connection */
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _connection, &ev) == -1) {
						message("epoll_ctl: conn_sock", "WARNING");
						break;
					}

				}
			}

			// excisting connection with a problem
			else if (all_events[i].events & EPOLLERR) {
				message("EPOLLERR received", "WARNING");
				// close connection with client
            	close(fd);
       		}

			// excisting connection sending or receiving data
			else {
				message("fd != server_fd", "INFO");\
				

			}
		}
	}

	// close epoll when done
	if (close(epoll_fd) < 0) {
		message("close(epoll_fd) ", "ERROR");
		exit(EXIT_FAILURE);
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