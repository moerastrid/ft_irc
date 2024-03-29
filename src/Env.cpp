/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Env.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 13:34:54 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 13:51:48 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Env.hpp"

Env::Env() {
	Msg("env - default constructor", "CLASS");
}
Env::Env(const Env &src) {
	*this = src;
	Msg("env - copy constructor", "CLASS");
}
Env &Env::operator=(const Env &src) {
	(void)src;
	Msg("env - assignation operator", "CLASS");
	return (*this);
}
Env::~Env() {
	Msg("env - default destructor", "CLASS");
}

Env::Env(int port, string pass) : pass(pass), port(port) {
	Msg("env - constructor(port, pass)", "CLASS");
};

Client&	Env::getClientByFD(int fd) const{
	for (auto& el : this->clients) {
		if (el->getPFD().fd == fd)
			return *el;
	}
	return Client::nullclient;
}

Client&	Env::getClientByNick(const string& nick) {
	for (auto& el : this->clients) {
		if (el->getNickname() == nick)
			return *el;
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

deque<Client *>::iterator	Env::getItToClientByFD(int fd) {
	for (deque<Client *>::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((*it)->getFD() == fd)
			return (it);
	}
	return clients.end();
}

deque<Channel>::iterator	Env::getItToChannelByName(const string& name) {
	for (deque<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		if (it->getName() == name)
			return it;
	}
	return channels.end();
}

const deque<Client *>&	Env::getClients() const {
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

void	Env::setHostname(const string &hostname) {
	this->hostname = hostname;
}

void	Env::setIP(const string &ip) {
	this->ip = ip;
}

void	Env::addClient(int fd) {
	Client *pointer = NULL;
	pointer = new Client(fd);
	this->clients.emplace_back(pointer);
}

void	Env::removeClient(int fd) {
	if (clients.empty())
	{
		Msg("BIG ERROR!", "ERROR");
		return ;
	}
	for (deque<Client *>::iterator it = clients.begin(); it != clients.end(); it++) {
		if ((*(*it)).getFD() == fd) {
			delete *it;
			clients.erase(it);
			break ;
		}
	}
}

void	Env::reset()
{
	if (!this->clients.empty()) {
		for (deque<Client *>::iterator it = this->clients.begin(); it != this->clients.end(); it++) {
			close((*it)->getFD());
			delete *it;
		}
		this->clients.clear();
	}
	if (!this->channels.empty())
		this->channels.clear();
}


std::ostream& operator<<(std::ostream& os, const Env& e) {
	os << "env(" << e.getHostname() << ", " << e.getIP() << ", "<< e.getPort() << ")";
	return os;
}
