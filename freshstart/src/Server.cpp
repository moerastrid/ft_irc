#include "Server.hpp"

/* 
TO DO (astrid)
	-	try-catch blokken maken voor server setup (& die koppelen aan main?)
	-	when shutting down server (???) (closing client connections?)

*/

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
	if (this != &src) {
		_sockin = src._sockin;
		_sockfd = src._sockfd;
		_port = src._port;
		_pass = src._pass;
		_hostname = src._hostname;
		_ip = src._ip;
		_clients = src._clients;
	}
	return (*this);
}

Server::Server(const int port, const string pass) : _port(port), _pass(pass) {
	Msg("Server constructor (PORT, pass)", "DEBUG");
	memset(&_sockin, 0, sizeof(_sockin));
	memset(&_sockfd, 0, sizeof(_sockfd));

	_sockfd.events = POLLIN|POLLHUP;
	_sockin.sin_family = AF_INET;
	_sockin.sin_port = htons(_port);
	_sockin.sin_addr.s_addr = INADDR_ANY;

	Server::setUp();
	Server::setInfo();

	Msg("server waiting for connections ... ", "INFO");
}

void	Server::setInfo() {
	char	hostname[MAXHOSTNAMELEN];
	bzero(hostname, sizeof(hostname));
	if (gethostname(hostname, MAXHOSTNAMELEN) != 0) {
		perror("Server::setHostInfo gethostname");
		exit(EXIT_FAILURE);
	}
	_hostname = hostname;
	struct hostent *host_entry;
	host_entry = gethostbyname(hostname);
	if (host_entry == NULL) {
		perror("Server::setHostInfo gethostbyname");
		exit(EXIT_FAILURE);
	}
	string ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	_ip = ip;
}

void	Server::setUp() {
	_sockfd.fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (_sockfd.fd == -1) {
		perror("ERROR\tServer::setUp socket()");
		exit(EXIT_FAILURE);
	}
	fcntl(_sockfd.fd, F_SETFL, O_NONBLOCK);
	int yes = 1;
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


Client* Server::getClientByFD(int fd) {
	vector<Client>& clients = this->_clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return &(*it);
	}
	return NULL;
}

vector<Client>::iterator	Server::getItToClientByFD(int fd) {
	vector<Client>& clients = this->_clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return (it);
	}
	return clients.end();
}

void	Server::addConnection() {
	socklen_t	tempSize = sizeof(_sockin);
	int new_fd;
	memset(&new_fd, 0, sizeof(new_fd));

	new_fd = accept(_sockfd.fd, (struct sockaddr *)&_sockin, (socklen_t *)&tempSize);
	if (new_fd == -1) {
			perror("accept");
			return ;
	} else {
		Msg("Connection accepted on " + std::to_string(new_fd), "INFO");
		_clients.push_back(Client(new_fd));
	}
}

void	Server::closeConnection(const int fd) {
	Msg("Connection closed on " + std::to_string(fd), "INFO");
	close(fd);
	_clients.erase(getItToClientByFD(fd));
}



void	Server::run(Executor& ex) {
	if (setPoll() == 0)
		return;

	CustomOutputStream customOut(cout);
	
	if (_sockfd.revents & POLLIN) {
		addConnection();
	} else if (_sockfd.revents & POLLERR ) {
		Msg("HELP", "ERROR");
		exit(-1) ;
	}
	
	/* bool checkEvent(short& event);
	bool checkRevent(short& revent); */

	for (size_t	i = 0; i < _clients.size(); i++) {
		if (_clients[i].getPFD().revents == 0) {
			continue ;
		} else if (_clients[i].checkRevent(POLLHUP|POLLRDHUP)) {
			Msg("POLLHUP/POLLRDHUP", "DEBUG");
	 		closeConnection(_clients[i].getFD());
		} else if (_clients[i].checkRevent(POLLERR)) {
			Msg("POLLERR", "DEBUG");
	 		closeConnection(_clients[i].getFD());
		} else {
			if (_clients[i].checkRevent(POLLIN)) {
				Msg("POLLIN", "DEBUG");
				receiveChunk(_clients[i]);
			}
			if (_clients[i].checkRevent(POLLOUT)) {
				Msg("POLLOUT", "DEBUG");
				sendChunk(_clients[i]);
			}
		}
	}
}

void	Server::receiveChunk(Client &c) {
	char	buf[BUFSIZE];
	memset(&buf, 0, sizeof(buf));

	// recv 
}

void	Server::sendChunk(Client &c) {
	char	buf[BUFSIZE];
	memset(&buf, 0, sizeof(buf));
}



		// if (_clients[i].getPFD().revents & POLLIN) {
		// 	Msg("POLLIN", "DEBUG");
			
		// 	//client.recbuf = receive;
		// 	//if (newline -> execute)


		// 	buf += receive(_clients[i].getFD());

		// 	if (buf.find('\n') == string::npos)
		// 		continue;

		// 	vector<string> lines; //Split lines
		// 	istringstream iss(buf);
		// 	string line;
		// 	while (std::getline(iss, line, '\n')) {
		// 		lines.push_back(line + '\n');
		// 	}

		// 	for (string lin : lines) {
		// 		cout << "  Processing: [" << lin << "]" << endl;
		// 		Command cmd(lin);
		// 		string reply = ex.run(cmd, _clients[i].getFD());
		// 		customOut << "Server reply: [" << reply << "]" << endl;
		// 	}
		// }
		// if (_clients[i].getPFD().revents & POLLOUT) {
		// 	Msg("POLLOUT", "DEBUG");
		// 	char	hello[] = "Hello this is patrick ";
		// 	send(_clients[i].getFD(), hello, sizeof(hello), MSG_DONTWAIT);
		// } 




string Server::receive(int fd) {
	char	buf[BUFSIZE];
	string	received;
	Msg("incoming message from fd " + to_string(fd), "DEBUG");
	memset(&buf, 0, sizeof(buf));
	int nbytes = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
	if (nbytes == -1)
		return (NULL);
	// buf[nbytes] = '\0';
	received.append(buf);
	if (!received.empty()) {
		cout << received;
	} else {
		Msg("received empty string", "DEBUG");
		closeConnection(fd);
	}
	// while (nbytes != 0) {
	// 	memset(&buf, 0, sizeof(buf));
	// 	nbytes = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
	// 	received.append(buf);
	// }
	return (received);
}

int	Server::setPoll() {
	vector<struct pollfd>	pollFds;

	pollFds.push_back(_sockfd);
	for (const auto& client : _clients) {
		pollFds.push_back(client.getPFD());
	}
	int ret = poll(pollFds.data(), pollFds.size(), -1);
	if (ret < 0) {
		perror("ERROR\tpoll :");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		Msg("None of the FD's are ready", "INFO");
	}

	for (const auto &pollFd : pollFds) {
		if (pollFd.fd == _sockfd.fd) {
			_sockfd.revents = pollFd.revents;
		} else {
			Client *C = getClientByFD(pollFd.fd);
			C->setRevents(pollFd.revents);
		}
	}
	return(ret);
}

const string	Server::getHostname() const {
	return _hostname;
}

const string	Server::getIP() const {
	return _ip;
}

int	Server::getPort() const {
	return _port;
}

const string	Server::getName() const {
	return _name;
}

std::ostream& operator<<(std::ostream& os, const Server& serv) {
	os << "Server(" << serv.getName() << ", " << serv.getHostname() << ", " << serv.getIP() << ", " << serv.getPort() << ")";
	return os;
}