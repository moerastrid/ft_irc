#include "server.hpp"
#include <cstring>

server::server() {}

server::~server() {
	if (close(_sockfd) < 0) {
		message("close(_sockfd) ", "ERROR");
		exit(EXIT_FAILURE);
	}
	if (close(_epfd) < 0) {
		message("close(_epfd) ", "ERROR");
		exit(EXIT_FAILURE);
	}
}

server::server(const server &src) {
	*this = src;
}

server &server::operator=(const server &src) {
	_port = src._port;
	_sockfd = src._sockfd;
	// _connection = src._connection;
	return (*this);
}


server::server(const unsigned int port, std::string password) : _port(port), _password(password) {
	
	
	_sockfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
	if (_sockfd < 0) {
		message("cannot create socket", "ERROR");
		exit(EXIT_FAILURE);
	} else {
		message("socket created", "INFO");
		std::cout << "socket fd : " << _sockfd << std::endl;
	}

	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&_opt, sizeof(_opt)) == -1) {
		message("cannot set socket opt", "ERROR");
		close(_sockfd);
		exit(EXIT_FAILURE);
	}

	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(_sockfd, ((const struct sockaddr *)&_address), sizeof(_address)) < 0) {
		message("Cannot bind socket", "ERROR");
		close(_sockfd);
		exit(EXIT_FAILURE) ;
	} else
		message("socket bind success", "INFO");

	if (listen(_sockfd, MAX_CONN) < 0) {
		message("Cannot hear socket", "ERROR");
		close(_sockfd);
		exit(EXIT_FAILURE) ;
	} else
		message("socket loud and clear over", "INFO");

	_epfd = epoll_create1(0);

	if (_epfd < 0) {
		message("epoll_fd", "ERROR");
		close(_sockfd);
		exit(EXIT_FAILURE);
	} else {
		message("epoll fd created", "INFO");
		std::cout << "epoll fd : " << _epfd << std::endl;
	}

	_event.events = EPOLLIN|EPOLLOUT|EPOLLET; /*epoll_wait always reports EPOLLERR so we don't need to specify it here */
	_event.data.fd = _sockfd;

	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _sockfd, &_event) < 0) {
		message("epoll_ctl", "ERROR");
		close(_sockfd);
		close(_epfd);
		exit(EXIT_FAILURE);
	} else {
		message("connection added", "INFO");
	}
}

void server::run() {
	message("start while loop", "DEBUG");
	int	nfds = epoll_wait(_epfd, _events, MAX_EVENT, _timeout);
	if (nfds < 0) {
		message("epoll_wait", "ERROR");
		exit(EXIT_FAILURE);
	}
	if (nfds == 0) {
		message("no connect/communication made or none of the fd's are ready", "INFO");
		return ;
	}
	for (int i = 0; i < nfds; ++i) 
	{
		int incoming_fd = _events[i].data.fd;
		std::cout << "fd: " << incoming_fd << std::endl;

		if (incoming_fd == _sockfd)
		{
			connection	incoming(_sockfd);
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				break ;
			}
			if (incoming.get_fd() < 0) {
				message("cannot accept incoming connection on server socket fd", "WARNING");
				break ;
			}
			if (epoll_ctl(_epfd, EPOLL_CTL_ADD, incoming.get_fd(), &_event) == -1) {
				message("epoll_ctl: conn_sock", "WARNING");
				break;
			}
			message("connection added to epoll", "INFO");
			_cons.push_back(incoming);
			std::string welcom = ":localhost 001 blabla :Welcome to the Internet Relay Network username@localhost\n";
			send(incoming.get_fd(), welcom.c_str(), welcom.size(), 0);
		} else if (_events[i].events & EPOLLERR) {
			message("EPOLLERR received", "WARNING");
			close(incoming_fd);
		} else {
			message("fd != server_fd", "INFO");
			std::cout << "HELLO THIS IS HAPPENING!!!" << std::endl;
			char buf[100000];
			bzero(buf, sizeof(buf));
				int n = read(_events[i].data.fd, buf,
					sizeof(buf));
				std::cout << "buf : " << std::endl;
				if (n <= 0 /* || errno == EAGAIN */ ) {
					break;
				} else {
					printf("[+] data: %s\n", buf);
					write(_events[i].data.fd, buf,
						strlen(buf));
				}
		}
	}

	// socklen_t len = sizeof(_address);

	// begin loop? voor meerdere _connections
	
		// _connection = accept(_sockfd, (struct sockaddr *)&_address, &len);
		// if (_connection < 0) {
		// 	// std::cerr << "Cannot accept socket" << std::endl;
		// 		(void)_connection;
		// 	} else {
		// 	std::cout << "socket " << _sockfd << " accepted" << std::endl;

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

