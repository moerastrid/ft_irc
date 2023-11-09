#include "Server.hpp"
#include "Msg.hpp"

Server::Server() {
	Msg("Server default constructor", "DEBUG");
}

Server::~Server() {
	Msg("Server default destructor", "DEBUG");

	close(_sockfd.fd);
}

Server::Server(const Server &src) {
	Msg("Server copy contructor", "DEBUG");
	*this = src;
}

Server &Server::operator=(const Server &src) {
	Msg("Server assignment operator", "DEBUG");
	_sockin = src._sockin;
	_sockfd = src._sockfd;
	_port = src._port;
	_pass = src._pass;
	_pollFds = src._pollFds;
	return (*this);
}

Server::Server(const int port, const string pass) : _port(port), _pass(pass) {
	Msg("Server constructor (PORT, pass)", "DEBUG");
	memset(&_sockin, 0, sizeof(_sockin));
	memset(&_sockfd, 0, sizeof(_sockfd));

	_sockfd.events = POLLIN|POLLOUT|POLLHUP;

	_sockin.sin_family = AF_INET;
	_sockin.sin_port = htons(_port);
	_sockin.sin_addr.s_addr = INADDR_ANY; /*perhaps htonl(INADDR_ANY); instead?*/

	Server::setUp();
	_pollFds.push_back(_sockfd);

	Msg("server waiting for connections ... ", "INFO");
}

void	Server::setUp() {
	_sockfd.fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (_sockfd.fd == -1) {
		perror("ERROR\tServer::setUp socket()");
    	exit(EXIT_FAILURE);
	}
	int yes = 1;
	fcntl(_sockfd.fd, F_SETFL, O_NONBLOCK);
	if (setsockopt(_sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("ERROR\tServer::setUp setsockopt()");
		exit(EXIT_FAILURE);
	}
	if (bind(_sockfd.fd, (struct sockaddr *) &_sockin, sizeof(_sockin)) == -1) {
		if (_port < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
    	perror("ERROR\tServer::setUp bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(_sockfd.fd, SOMAXCONN) == -1) {
		perror("ERROR\tServer::setUp listen()");
		exit(EXIT_FAILURE);
	}
}

void	Server::addConnection() {
	socklen_t	tempSize = sizeof(_sockin);
	struct pollfd new_fd;
	memset(&new_fd, 0, sizeof(new_fd));

	new_fd.fd = accept(_sockfd.fd, (struct sockaddr *)&_sockin, (socklen_t *)&tempSize);
	if (new_fd.fd == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			// Msg("No pending connections", "INFO");
			return ;
		} else {
			perror("accept");
			return ;
		}
	} else {
		Msg("Connection accepted on " + std::to_string(new_fd.fd), "INFO");
		new_fd.events = POLLIN|POLLOUT|POLLHUP|POLLRDHUP;
		_pollFds.push_back(new_fd);

		// Client(new_fd.fd);
	}
}

void	Server::closeConnection(const int i) {
	close(_pollFds[i].fd);			
	_pollFds.erase(_pollFds.begin() + i );
}



void	Server::run() {
	if (setPoll() == 0)
		return;
	

	for (unsigned long i(0); i < _pollFds.size(); i++) {
		if (_pollFds[i].fd == _sockfd.fd && _pollFds[i].revents == POLLIN) {
			addConnection();
		}
		else {
			if (_pollFds[i].revents == 0){
				continue;
			} else if (_pollFds[i].revents & POLLIN) {
				Msg("POLLIN", "DEBUG");
				receive(_pollFds[i].fd);
				// sleep(1);
			} else if (_pollFds[i].revents & POLLOUT) {
				Msg("POLLOUT", "DEBUG");
				// sleep(1);
			} else if (_pollFds[i].revents & (POLLHUP | POLLRDHUP)) {
				Msg("POLLHUP or POLLRDHUP", "DEBUG");
				closeConnection(i);
			}

			// switch(_pollFds[i].revents) {
			// 	case 0:
			// 		// Msg("case 0", "DEBUG");					
			// 		break ;

			// 	case POLLIN:
			// 		Msg("POLLIN", "DEBUG");
			// 		receive(_pollFds[i].fd);
					
			// 		break ;

			// 	case POLLOUT:
			// 		Msg("POLLOUT", "DEBUG");
			// 		sleep(1);
			// 		break ;

			// 	case POLLHUP:
			// 	case POLLRDHUP:
			// 		Msg("POLLHUP or POLLRDHUP", "DEBUG");
			// 		closeConnection(i);
			// 		break ;

			// }
		}
	}
}

void Server::receive(int fd) {
	char	buf[BUFSIZE];
	string	received;
	Msg("incoming message :", "DEBUG");
	memset(&buf, 0, sizeof(buf));
	int nbytes = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
	buf[nbytes] = '\0';
	received.append(buf);
	cout << received << endl;
}

int	Server::setPoll() {
	int ret = poll(_pollFds.data(), _pollFds.size(), 1000);
	if (ret < 0) {
		perror("ERROR\tpoll :");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		Msg("None of the FD's are ready", "INFO");
	}
	return(ret);
}

const string	Server::getIP() const {
	char	hostname[MAXHOSTNAMELEN];
	bzero(hostname, sizeof(hostname));
	if (gethostname(hostname, MAXHOSTNAMELEN) != 0) {
		perror("Server::setHostInfo gethostname");
    	exit(EXIT_FAILURE);
	}

	struct hostent *host_entry;
	host_entry = gethostbyname(hostname);
   	if (host_entry == NULL) {
    	perror("Server::setHostInfo gethostbyname");
    	exit(EXIT_FAILURE);
   	}
	string ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	return (ip);
}

int	Server::getPort() const {
	return this->_port;
}

std::ostream& operator<<(std::ostream& os, const Server& serv) {
	os << "Server(" << serv.getIP() << ", " << serv.getPort() << ")";
	return os;
}