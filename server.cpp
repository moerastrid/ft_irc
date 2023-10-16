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
	// server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
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
	
	socklen_t len = sizeof(this->_address);

	// begin loop? voor meerdere _connections
	while (1) {
		_connection = accept(_server_fd, (struct sockaddr *)&_address, &len);
		if (_connection < 0) {
			// std::cerr << "Cannot accept socket" << std::endl;
				(void)_connection;
			} else {
			std::cout << "socket " << _server_fd << " accepted" << std::endl;

			// receive info
			char buf[40000];
			long valread = read(_connection, buf, 40000);
			std::cout << buf << std::endl;
			std::cout << "valread : " << valread << std::endl;

			// send message
			std::string response = "this is FT_IRC over \n";
			
			send(_connection, response.c_str(), response.size(), 0);
			std::cout << "hello message sent" << std::endl;
			}
		// pas helemaal als je klaar bent:
		close(_connection);
	}
}