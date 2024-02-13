/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 18:15:47 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 19:34:22 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//private

void	Server::setUp() {
	this->sockfd.fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (this->sockfd.fd == -1)
		throw ServerException("error in Server::setUp - socket");
	fcntl(this->sockfd.fd, F_SETFL, O_NONBLOCK);
	int yes = 1;
	if (setsockopt(this->sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) //yes to NULL
		throw ServerException("error in Server::setUp - setsockopt");
	if (bind(this->sockfd.fd, (struct sockaddr *) &this->sockin, sizeof(this->sockin)) == -1) {
		if (this->e.getPort() < 1024)
			Msg("If you're not superuser: no permission to use ports under 1024", "WARNING");
		throw ServerException("error in Server::setUp - bind");
	}
	if (listen(this->sockfd.fd, SOMAXCONN) == -1)
		throw ServerException("error in Server::setUp - listen");
}
void	Server::setInfo() {
	char	hostname[MAXHOSTNAMELEN];
	bzero(hostname, sizeof(hostname));
	if (gethostname(hostname, MAXHOSTNAMELEN) != 0)
		throw ServerException("error inServer::setInfo - gethostname");
	this->e.setHostname(hostname);
	struct hostent *host_entry;
	host_entry = gethostbyname(hostname);
	if (host_entry == NULL)
		throw ServerException("error inServer::setInfo - gethostbyname");
	string ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	this->e.setIP(ip);
}
int	Server::setPoll() {
	vector<struct pollfd>	pollFds;

	pollFds.push_back(sockfd);
	for (const Client* client : this->e.getClients()) {
		pollFds.push_back(client->getPFD());
	}
	int ret = poll(pollFds.data(), pollFds.size(), -1);
	if (ret < 0) {
		if (errno == EINTR)
			Msg("poll returned -1", "INFO");
		else
			throw ServerException("error in Server::setPoll - poll");
		return (0);
	} else if (ret == 0) {
		return (ret);
	}

	for (const auto &pollFd : pollFds) {
		if (pollFd.fd == sockfd.fd) {
			sockfd.revents = pollFd.revents;
		} else {
			Client& c = this->e.getClientByFD(pollFd.fd);
			c.setRevents(pollFd.revents);
		}
	}
	return(ret);
}

void	Server::addConnection() {
	socklen_t	tempSize = sizeof(sockin);
	int new_fd;
	memset(&new_fd, 0, sizeof(new_fd));

	new_fd = accept(sockfd.fd, (struct sockaddr *)&sockin, (socklen_t *)&tempSize);
	if (new_fd == -1) 
		throw ServerException("error in Server::addConnection - accept");
	else {
		Msg("Connection accepted on " + std::to_string(new_fd), "INFO");
		this->e.addClient(new_fd);
	}
}
void	Server::closeConnection(const int fd) {
	std::deque<Channel>& channels = this->e.getChannels();
	Client& client = this->e.getClientByFD(fd);

	for (Channel& channel : channels) {
		if (channel.hasMember(client)) {
			channel.sendMessageToOtherMembers(client, ":" + (client).getFullName() + " QUIT :lost connection\r\n");
			channel.removeMember(client);
		}
	}
	auto newEnd = std::remove_if(channels.begin(), channels.end(), channel_is_empty);
	channels.erase(newEnd, channels.end());
	
	Msg("Connection closed on " + std::to_string(fd), "INFO");
	this->e.removeClient(fd);

	close(fd);
}

bool	Server::receivefromClient(Client &c) {
	char	buf[BUFSIZE];
	memset(&buf, 0, sizeof(buf));

	int nbytes = recv(c.getFD(), buf, sizeof(buf) - 1, MSG_DONTWAIT);
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
	int nbytes = send(c.getFD(), dataToSend.c_str(), dataToSend.size(), 0);
	if (nbytes <= 0) {
		Msg("error in sending data", "ERROR");
		return (false);
	}
	Msg("Sending: [" + dataToSend + "]", "STREAM");
	return (true);
}

bool	Server::comm_pollin(Executor& ex, Client &client) {
	if (receivefromClient(client) == false) {
		closeConnection(client.getFD());
		return false;
	}

	while (client.hasRecvData()) {
		string receiveData = client.takeRecvData();
		Msg("Executing [" + receiveData + "]", "STREAM");
		Command cmd(receiveData);
		if (ex.run(cmd, client) == false) {
			if (!client.hasSendData())
				closeConnection(client.getFD());
			else
				client.expell();
			return false;
		}
	}
	return true ;
}
void	Server::comm_pollout(Client &client) {
	if (sendtoClient(client) == false) {
		closeConnection(client.getFD());
	}
	if (!client.hasSendData())
	{
		client.removeEvent(POLLOUT);
		if (client.isExpelled())
			closeConnection(client.getFD());
	}
}


//public
Server::Server(Env& e) : e(e) {
	Msg("Server - constructor (e)", "CLASS");
	memset(&this->sockin, 0, sizeof(this->sockin));
	memset(&this->sockfd, 0, sizeof(this->sockfd));

	this->sockfd.events = POLLIN|POLLHUP|POLLRDHUP;
	this->sockin.sin_family = AF_INET;
	this->sockin.sin_port = htons(this->e.getPort());
	this->sockin.sin_addr.s_addr = INADDR_ANY;

	Server::setUp();
	Server::setInfo();

	Msg("server waiting for connections ... ", "DEBUG");
}

Server::~Server() {
	Msg("Server - default destructor", "CLASS");
	close(sockfd.fd);
}

void	Server::run(Executor& ex) {
	if (setPoll() == 0)
		return;
	
	if (sockfd.revents & POLLIN) {
		addConnection();
		return ;
	} else if (sockfd.revents & POLLERR )
		throw ServerException("error in Server::run - sockfd.revents & POLLERR");

	const deque<Client *>&	clients = this->e.getClients();

	for (size_t	i = 0; i < clients.size(); i++) {
		Client& client = *clients[i];

		if (client.getPFD().revents == 0) {
			continue ;
		} else if (client.checkRevent(POLLHUP|POLLRDHUP)) {
			Msg("POLLHUP/POLLRDHUP", "DEBUG");
	 		closeConnection(client.getFD());
		} else if (client.checkRevent(POLLERR)) {
			Msg("POLLERR" + to_string(client.getFD()), "DEBUG");
	 		closeConnection(client.getFD());
		} else {
			if (!client.isExpelled() && client.checkRevent(POLLIN)) {
				Msg("POLLIN " + to_string(client.getFD()), "DEBUG");
				if (!comm_pollin(ex, client)) 
					continue ;
			}
			if (client.checkRevent(POLLOUT)) {
				Msg("POLLOUT " + to_string(client.getFD()), "DEBUG");
				comm_pollout(client);
			}
		}
	}
}

const string	Server::getName() const {
	return name;
}

std::ostream& operator<<(std::ostream& os, const Server& serv) {
	os << "Server(" << serv.getName() << ")";
	return os;
}
