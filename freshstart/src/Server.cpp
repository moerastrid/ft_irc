#include "Server.hpp"

/* 
TO DO (astrid)
	-	try-catch blokken maken voor server setup (& die koppelen aan main?)
*/

// Server::Server() {
// 	Msg("Server default constructor", "DEBUG");
// }


// #TODO delete
void printHex(const std::string& str) {
    for (char c : str) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    }
    std::cout << std::dec << std::endl;
}

Server::~Server() {
	Msg("Server default destructor", "DEBUG");
	close(sockfd.fd);
}

Server::Server(const Server &src) : env(src.env) {
	Msg("Server copy contructor", "DEBUG");
	*this = src;
}

Server &Server::operator=(const Server &src) {
	Msg("Server assignment operator", "DEBUG");
	if (this != &src) {
		this->env = src.env;
		this->sockin = src.sockin;
		this->sockfd = src.sockfd;
	}
	return (*this);
}

Server::Server(Env& env) : env(env) {
	Msg("Server constructor (env)", "DEBUG");
	memset(&this->sockin, 0, sizeof(this->sockin));
	memset(&this->sockfd, 0, sizeof(this->sockfd));

	this->sockfd.events = POLLIN|POLLHUP|POLLRDHUP;
	this->sockin.sin_family = AF_INET;
	this->sockin.sin_port = htons(this->env.port);
	this->sockin.sin_addr.s_addr = INADDR_ANY;

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
	this->sockfd.fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (this->sockfd.fd == -1) {
		perror("ERROR\tServer::setUp socket()");
		exit(EXIT_FAILURE);
	}
	fcntl(this->sockfd.fd, F_SETFL, O_NONBLOCK);
	int yes = 1;
	if (setsockopt(this->sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("ERROR\tServer::setUp setsockopt()");
		exit(EXIT_FAILURE);
	}
	if (bind(this->sockfd.fd, (struct sockaddr *) &this->sockin, sizeof(this->sockin)) == -1) {
		if (this->env.port < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
		perror("ERROR\tServer::setUp bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(this->sockfd.fd, SOMAXCONN) == -1) {
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

void	Server::addConnection() { // This or the function that calls it should probably check if a connection for the same user already exists.
	socklen_t	tempSize = sizeof(sockin);
	int new_fd;
	memset(&new_fd, 0, sizeof(new_fd));

	new_fd = accept(sockfd.fd, (struct sockaddr *)&sockin, (socklen_t *)&tempSize);
	if (new_fd == -1) {
			perror("accept");
			return ;
	} else {
		Msg("Connection accepted on " + std::to_string(new_fd), "INFO");
		Client temp(new_fd);
		this->env.clients.push_back(temp);
	}
}

void	Server::closeConnection(const int fd) {
	Msg("Connection closed on " + std::to_string(fd), "INFO");
	close(fd);
	this->env.clients.erase(getItToClientByFD(fd));
}

#include <string>
#define BG_COLOR_MAGENTA "\033[45m" // #TODO delete
#define COLOR_GREEN "\033[32m"
#define COLOR_RESET "\033[0m"

void	Server::run(Executor& ex) {
	if (setPoll() == 0)
		return;
	
	if (sockfd.revents & POLLIN) {
		addConnection();
		return ;
	} else if (sockfd.revents & POLLERR ) {
		Msg("HELP", "ERROR");
		exit(-1) ;
	}

	for (size_t	i = 0; i < this->env.clients.size(); i++) {
		Client& client = this->env.clients[i];

		if (client.getPFD().revents == 0) {
			continue ;
		} else if (client.checkRevent(POLLHUP|POLLRDHUP)) {
			Msg("POLLHUP/POLLRDHUP", "DEBUG");
	 		closeConnection(client.getFD());
		} else if (client.checkRevent(POLLERR)) {
			Msg("POLLERR", "DEBUG");
	 		closeConnection(client.getFD());
		} else {
			if (client.checkRevent(POLLIN)) {
				Msg("POLLIN", "DEBUG");
				if (receivefromClient(client) == false)
					closeConnection(client.getFD());
				// execute?

				// Execute:
				while (client.hasRecvData()) {
					string receiveData = client.takeRecvData(); // Get the line. 
					this->customOut << BG_COLOR_MAGENTA << "EXECUTING: [" << receiveData << "]" << COLOR_RESET << endl; // #TODO delete
					Command cmd(receiveData);					// Turn it into a command.
					if (ex.run(cmd, client) == false)			// Run the command.
						closeConnection(client.getFD());
				}
				if (client.hasSendData())
					client.addEvent(POLLOUT);
			}
			if (client.checkRevent(POLLOUT)) {
				Msg("POLLOUT", "DEBUG");
				if (sendtoClient(client) == false)
					closeConnection(client.getFD());

				if (!client.hasSendData()) //check for more messages. If none, remove event.
					client.removeEvent(POLLOUT);
			}
		}
	}
}

bool	Server::receivefromClient(Client &c) {
	char	buf[BUFSIZE];
	memset(&buf, 0, sizeof(buf));

	int nbytes = recv(c.getFD(), buf, sizeof(buf), MSG_DONTWAIT);
	if (nbytes < 0) {
		Msg("error in receiving data", "ERROR"); 
		return (false);
	}
	if (nbytes == 0) {
		Msg("no connection with Client " + to_string(c.getFD()), "WARNING"); 
		return (false);
	}
	c.addRecvData(string(buf));

	return (true);
}

bool	Server::sendtoClient(Client &c) {
	string	dataToSend = c.takeSendData();
	if (dataToSend.empty()) {
		c.setEvents(POLLIN|POLLHUP|POLLRDHUP);
		return (true);
	}
	// dataToSend = ":" + this->env.hostname + " " + dataToSend; // Leaving this out for now, there's an issue (PONG)
	int nbytes = send(c.getFD(), dataToSend.c_str(), dataToSend.size(), 0);
	if (nbytes <= 0) {
		Msg("error in sending data", "ERROR");
		return (false);
	}
	this->customOut << COLOR_GREEN << "Sending: [" << dataToSend << "]" << COLOR_RESET << endl; //#TODO delete
	return (true);
}

int	Server::setPoll() {
	vector<struct pollfd>	pollFds;

	pollFds.push_back(sockfd);
	for (const auto& client : this->env.clients) {
		pollFds.push_back(client.getPFD());
	}
	int ret = poll(pollFds.data(), pollFds.size(), -1);
	if (ret < 0) {
		// perror("ERROR\tpoll :");
		// exit(EXIT_FAILURE);
		return (0);
	} else if (ret == 0) {
		Msg("None of the FD's are ready", "INFO");
	}

	for (const auto &pollFd : pollFds) {
		if (pollFd.fd == sockfd.fd) {
			sockfd.revents = pollFd.revents;
		} else {
			Client *c = getClientByFD(pollFd.fd);
			c->setRevents(pollFd.revents);
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

CustomOutputStream Server::customOut(std::cout);
CustomOutputStream Server::customErr(std::cerr);
// std::ofstream Server::outputfile("output.txt");

