#include "Client.hpp"

#include <iostream>
using std::cout;
using std::endl;

Client::Client(int fd) : fd(fd) {};

Client::~Client() {}

Client::Client(const Client& other) : fd(other.fd) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
	}
	return *this;
}

void Client::setNickname(string& nickname) {
	this->nickname = nickname;
}
void Client::setUsername(string& username) {
	this->username = username;
}
void Client::setHostname(string& hostname) {
	this->hostname = hostname;
}
void Client::setServername(string& servername) {
	this->servername = servername;
}
void Client::setRealname(string& realname) {
	this->realname = realname;
}
void Client::makeOperator(Channel& c) {
	c.addOperator(*this);
}
void Client::takeOperator(Channel& c) {
	c.removeOperator(*this);
}

int Client::getFD() const {
	return this->fd;
}
const string& Client::getNickname() const {
	return this->nickname;
}
const string& Client::getUsername() const {
	return this->username;
}
const string& Client::getHostname() const {
	return this->hostname;
}
const string& Client::getServername() const {
	return this->servername;
}
const string& Client::getRealname() const {
	return this->realname;
}
bool Client::isOperator(Channel& c) const {
	return c.hasOperator(*this);
}
bool Client::isFounder(Channel& c) const {
	return c.hasFounder(*this);
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
	string str = string("Client(") + std::to_string(client.getFD()) + ", " + client.getNickname() + ", " + client.getUsername() + ", " + client.getHostname() + ", " + client.getServername() + ", " + client.getRealname() + ")";
	os << str;
	return os;
}

bool operator==(const Client &lhs, const Client &rhs) {
	return 	lhs.getFD() == rhs.getFD() &&
			lhs.getNickname() == rhs.getNickname() &&
			lhs.getUsername() == rhs.getUsername();
}
