#include "Server.hpp"
#include "Msg.hpp"

Server::Server() {
	Msg("Server default constructor", "DEBUG");
}

Server::~Server() {
	Msg("Server default destructor", "DEBUG");

	freeaddrinfo(_addrinfo);
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

	int status = getaddrinfo(_ip.c_str(), std::to_string(port).c_str(), &hints, &_addrinfo);
	if (status != 0) {
		Msg(gai_strerror(status), "ERROR");
		exit(EXIT_FAILURE);
	}
}

void	Server::setUp() {
	_fd = socket(_addrinfo->ai_family, _addrinfo->ai_socktype, _addrinfo->ai_protocol);
	if (_fd == -1) {
		perror("Server::setUp socket()");
    	exit(EXIT_FAILURE);
	}
	if (bind(_fd, _addrinfo->ai_addr, _addrinfo->ai_addrlen) == -1) {
		if (_port < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
    	perror("ERROR\tServer::setUp bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(_fd, SOMAXCONN) == -1) {
		perror("ERROR\tServer::setUp listen()");
		exit(EXIT_FAILURE);
	}
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