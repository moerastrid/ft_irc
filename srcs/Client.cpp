#include "Client.hpp"

Client::Client(int fd) {
	this->pfd.fd = fd; // effectively const
	this->pfd.events = POLLIN|POLLOUT|POLLHUP|POLLRDHUP;
};

Client::~Client() {}

Client::Client(const Client& other) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
	this->pfd = other.pfd;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
		this->pfd = other.pfd;
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
void Client::setPassword(string& password) {
	this->password = password;
}
void Client::makeOperator(Channel& c) {
	c.addOperator(*this);
}
void Client::takeOperator(Channel& c) {
	c.removeOperator(*this);
}

int Client::getFD() const {
	return this->pfd.fd;
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

ostream& operator<<(ostream& os, const Client& client) {
	string str = string("Client(") + to_string(client.getFD()) + ", " + client.getNickname() + ", " + client.getUsername() + ", " + client.getHostname() + ", " + client.getServername() + ", " + client.getRealname() + ")";
	os << str;
	return os;
}

bool operator==(const Client &lhs, const Client &rhs) {
	return 	lhs.getFD() == rhs.getFD() &&
			lhs.getNickname() == rhs.getNickname() &&
			lhs.getUsername() == rhs.getUsername();
}
