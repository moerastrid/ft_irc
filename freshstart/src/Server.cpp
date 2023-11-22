#include "Server.hpp"

// Server::Server() {
// 	Msg("Server default constructor", "DEBUG");
// }

Server::~Server() {
	Msg("Server default destructor", "DEBUG");

	close(env.sockfd.fd);
}

Server::Server(const Server &src) : env(src.env) {
	Msg("Server copy contructor", "DEBUG");
	*this = src;
}

Server &Server::operator=(const Server &src) {
	Msg("Server assignment operator", "DEBUG");
	if (this != &src) {
		this->env = src.env;
		// _sockin = src._sockin;
		// _sockfd = src._sockfd;
		// _port = src._port;
		// _pass = src._pass;
		// _hostname = src._hostname;
		// _ip = src._ip;
		// this->env.clients = src._clients;
	}
	return (*this);
}

Server::Server(Env& env) : env(env) {
	Msg("Server constructor (PORT, pass)", "DEBUG");
	// memset(&env.sockin, 0, sizeof(env.sockin));
	// memset(&env.sockfd, 0, sizeof(env.sockfd));

	// _sockfd.events = POLLIN|POLLHUP;
	// _sockin.sin_family = AF_INET;
	// _sockin.sin_port = htons(env.port);
	// _sockin.sin_addr.s_addr = INADDR_ANY; /*perhaps htonl(INADDR_ANY); instead?*/

	Server::setUp();
	Server::setInfo();
	// _pollFds.push_back(_sockfd);

	Msg("server waiting for connections ... ", "INFO");
}

void	Server::setInfo() {
	char	hostname[MAXHOSTNAMELEN];
	bzero(hostname, sizeof(hostname));
	if (gethostname(hostname, MAXHOSTNAMELEN) != 0) {
		perror("Server::setHostInfo gethostname");
		exit(EXIT_FAILURE);
	}
	this->env.hostname = hostname;
	struct hostent *host_entry;
	host_entry = gethostbyname(hostname);
	if (host_entry == NULL) {
		perror("Server::setHostInfo gethostbyname");
		exit(EXIT_FAILURE);
	}
	string ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	this->env.ip = ip;
}

void	Server::setUp() {
	this->env.sockfd.fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (this->env.sockfd.fd == -1) {
		perror("ERROR\tServer::setUp socket()");
		exit(EXIT_FAILURE);
	}
	// fcntl(_sockfd.fd, F_SETFL, O_NONBLOCK);
	int yes = 1;
	if (setsockopt(this->env.sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("ERROR\tServer::setUp setsockopt()");
		exit(EXIT_FAILURE);
	}
	if (bind(this->env.sockfd.fd, (struct sockaddr *) &this->env.sockin, sizeof(this->env.sockin)) == -1) {
		if (this->env.port < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
		perror("ERROR\tServer::setUp bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(this->env.sockfd.fd, SOMAXCONN) == -1) {
		perror("ERROR\tServer::setUp listen()");
		exit(EXIT_FAILURE);
	}
}


Client* Server::getClientByFD(int fd) {
	vector<Client>& clients = this->env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return &(*it);
	}
	return NULL;
}

vector<Client>::iterator	Server::getItToClientByFD(int fd) {
	vector<Client>& clients = this->env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return (it);
	}
	return clients.end();
}

void	Server::addConnection() {
	socklen_t	tempSize = sizeof(this->env.sockin);
	int new_fd;
	memset(&new_fd, 0, sizeof(new_fd));

	new_fd = accept(this->env.sockfd.fd, (struct sockaddr *)&this->env.sockin, (socklen_t *)&tempSize);
	if (new_fd == -1) {
			perror("accept");
			return ;
	} else {
		Msg("Connection accepted on " + std::to_string(new_fd), "INFO");
		this->env.clients.push_back(Client(new_fd));
	}
}

void	Server::closeConnection(const int fd) {
	Msg("Connection closed on " + std::to_string(fd), "INFO");
	close(fd);
	this->env.clients.erase(getItToClientByFD(fd));
	for (unsigned long i(0); i < this->env.clients.size(); i++) {
		if (this->env.clients[i].getFD() == fd)
			this->env.clients.erase(this->env.clients.begin() + i);
	}
}



void	Server::run(Executor& ex) {
	if (setPoll() == 0)
		return;

	CustomOutputStream customOut(cout);
	string buf = "";
	
	if (this->env.sockfd.revents & POLLIN) {
		addConnection();
	} else if (this->env.sockfd.revents & POLLERR ) {
		Msg("HELP", "ERROR");
		exit(-1) ;
	}
	
	/* bool checkEvent(short& event);
	bool checkRevent(short& revent); */

	for (size_t	i = 0; i < this->env.clients.size(); i++) {
		if (this->env.clients[i].getPFD().revents == 0)
			continue ;
		// if (this->env.clients[i].getPFD().revents & POLLHUP) {
		// 	Msg("POLLHUP", "DEBUG");
	 	// 	closeConnection(this->env.clients[i].getFD());
		// 	continue ;
		// }
		if (this->env.clients[i].checkRevent(POLLHUP|POLLRDHUP)) {
			Msg("POLLHUP/POLLRDHUP", "DEBUG");
	 		closeConnection(this->env.clients[i].getFD());
			continue ;
		}
		if (this->env.clients[i].getPFD().revents & POLLIN) {
			Msg("POLLIN", "DEBUG");
			
			//client.recbuf = receive;
			//if (newline -> execute)

			buf += receive(this->env.clients[i].getFD());

			if (buf.find('\n') == string::npos)
				continue;

			vector<string> lines; //Split lines
			istringstream iss(buf);
			string line;
			while (std::getline(iss, line, '\n')) {
				lines.push_back(line + '\n');
			}

			for (string lin : lines) {
				cout << "  Processing: [" << lin << "]" << endl;
				Command cmd(lin);
				string reply = ex.run(cmd, this->env.clients[i].getFD());
				customOut << "Server reply: [" << reply << "]" << endl;
			}
		}
		if (this->env.clients[i].getPFD().revents & POLLOUT) {
			Msg("POLLOUT", "DEBUG");
			char	hello[] = "Hello this is patrick ";
			send(this->env.clients[i].getFD(), hello, sizeof(hello), MSG_DONTWAIT);
		} 
	}
}


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

	pollFds.push_back(this->env.sockfd);
	for (const auto& client : this->env.clients) {
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
		if (pollFd.fd == this->env.sockfd.fd) {
			this->env.sockfd.revents = pollFd.revents;
		} else {
			Client *C = getClientByFD(pollFd.fd);
			C->setRevents(pollFd.revents);
		}
	}
	return(ret);
}

const string	Server::getHostname() const {
	return this->env.hostname;
}

const string	Server::getIP() const {
	return this->env.ip;
}

int	Server::getPort() const {
	return this->env.port;
}

const string	Server::getName() const {
	return name;
}

std::ostream& operator<<(std::ostream& os, const Server& serv) {
	os << "Server(" << serv.getName() << ", " << serv.getHostname() << ", " << serv.getIP() << ", " << serv.getPort() << ")";
	return os;
}