#include "Env.hpp"

Env::Env(int port, string pass) : pass(pass), port(port) {};

Client& Env::getClientByFD(int fd) {
	for(auto& el : this->clients) {
		if (el.getPFD().fd == fd)
			return el;
	}

	return Client::nullclient;
}

vector<Client>& Env::getClients() {
	return this->clients;
}

vector<Channel>& Env::getChannels() {
	return this->channels;
}

vector<Client>::iterator	Env::getItToClientByFD(int fd) {
	vector<Client>& clients = this->getClients();
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return (it);
	}
	return clients.end();
}

Client& Env::getClientByNick(const string& nick) {
	vector<Client>& clients = this->getClients();
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nick)
			return *it;
	}
	return Client::nullclient;
}

int	Env::getPort() const {
	return (this->port);
}

const string&	Env::getPass() const {
	return (this->pass);
}

const string&	Env::getHostname() const {
	return (this->hostname);
}

const string&	Env::getIP() const {
	return (this->ip);
}

//setters
void				Env::setHostname(const string &hostname) {
	this->hostname = hostname;
}

void				Env::setIP(const string &ip) {
	this->ip = ip;
}
