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
	(void)src;
	return (*this);
}

Server::Server(const int port, const std::string pass) : _port(port), _pass(pass) {
	Msg("Server constructor (PORT, pass)", "DEBUG");
	Server::setHostInfo();
	Server::setAddrInfo(port);
	Server::setUp();
}



void	Server::setHostInfo() {
	bzero(_hostname, sizeof(_hostname));
	if (gethostname(_hostname, MAXHOSTNAMELEN) != 0) {
		perror("Server::setHostInfo gethostname");
    	exit(EXIT_FAILURE);
	}

	struct hostent *host_entry;
	host_entry = gethostbyname(_hostname);
   	if (host_entry == NULL) {
    	perror("Server::setHostInfo gethostbyname");
    	exit(EXIT_FAILURE);
   	}
	_ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
}

void	Server::setAddrInfo(const int port) {
	struct addrinfo	hints;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &_addrinfo);
	if (status != 0) {
		Msg(gai_strerror(status), "ERROR");
		exit(EXIT_FAILURE);
	}
	_sockin.sin_family = AF_INET;
	_sockin.sin_port = htons(port);
	_sockin.sin_addr.s_addr = INADDR_ANY;
}

void	Server::setUp() {
	_sockfd.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd.fd == -1) {
		perror("Server::setUp socket()");
    	exit(EXIT_FAILURE);
	}
	fcntl(_sockfd.fd, F_SETFL, O_NONBLOCK);
	int yes = 1;
	if (setsockopt(_sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("ERROR\tServer::setUp setsockopt()");
		exit(EXIT_FAILURE);
	}
	if (bind(_sockfd.fd, (struct sockaddr *)&_sockin, sizeof(_sockin)) == -1) {
		if (_port < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
    	perror("ERROR\tServer::setUp bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(_sockfd.fd, SOMAXCONN) == -1) {
		perror("ERROR\tServer::setUp listen()");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(_addrinfo);
	Msg("server waiting for connections ... ", "INFO");
}

void	Server::run() {
	socklen_t sin_size;
	sin_size = sizeof(_their_addr);

	poll(&_sockfd, 1, 0);

	int new_fd = accept(_sockfd.fd, (struct sockaddr *)&_their_addr, (socklen_t*)&sin_size);
	if (new_fd == -1) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	// int num_events = poll(_pollfds, _num_pollfds, _polltime);
	// std::cout << "poll number of events : " << num_events << std::endl;
	// if (num_events != 0) {
	// 	int pollin_happened = _pollfds[0].revents & POLLIN;
	// 	if (pollin_happened)
	// 		Msg("Pollin", "DEBUG");
	// 	else
	// 		Msg("No Pollin :()", "DEBUG");
	// }
}

const std::string	Server::getIP() const {
	return this->_ip;
}

const std::string	Server::getHostname() const {
	return this->_hostname;
}

int	Server::getPort() const {
	return this->_port;
}

std::ostream& operator<<(std::ostream& os, const Server& serv) {
	os << "Server(" << serv.getIP() << ", " << serv.getHostname() << ", " << serv.getPort() << ")";
	return os;
}