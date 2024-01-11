#include "Env.hpp"
#include "Msg.hpp"

Env::Env(int port, string pass) : pass(pass), port(port) {
	Msg("env - constructor(port, pass)", "CLASS");
};

Client&	Env::getClientByFD(int fd) {
	for(auto& el : this->clients) {
		if (el.getPFD().fd == fd)
			return el;
	}
	return Client::nullclient;
}

Client&	Env::getClientByNick(const string& nick) {
	for (auto& el : this->clients) {
		if (el.getNickname() == nick)
			return el;
	}
	return Client::nullclient;
}

Channel&	Env::getChannelByName(const string& name) {
	for (Channel& ch : getChannels()) {
		if (ch.getName() == name) {
			return ch;
		}
	}
	return Channel::nullchan;
}

deque<Client>::iterator	Env::getItToClientByFD(int fd) {
	for (deque<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return (it);
	}
	return clients.end();
}

deque<Client>::iterator	Env::getItToClientByNick(string nick) {
	for (deque<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nick)
			return it;
	}
	return clients.end();
}

deque<Client>&	Env::getClients() {
	return this->clients;
}

deque<Channel>&	Env::getChannels() {
	return this->channels;
}

int	Env::getPort() const {
	return this->port;
}

const	string&	Env::getPass() const {
	return this->pass;
}

const	string&	Env::getHostname() const {
	return this->hostname;
}

const	string&	Env::getIP() const {
	return this->ip;
}

//setters
void	Env::setHostname(const string &hostname) {
	this->hostname = hostname;
}

void	Env::setIP(const string &ip) {
	this->ip = ip;
}
