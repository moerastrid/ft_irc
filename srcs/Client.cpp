#include "Client.hpp"

#include <iostream>
using std::cout;
using std::endl;

Client::Client(int fd) : fd(fd), operatorStatus(0) {};

Client::~Client() {}

Client::Client(const Client& other) : fd(other.fd) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
	this->operatorStatus = 0;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
		this->operatorStatus = other.operatorStatus;
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
void Client::makeOperator() {
	this->operatorStatus = 1;
}
void Client::takeOperator() {
	this->operatorStatus = 0;
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
bool Client::isOperator() const {
	return this->operatorStatus;
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
	string str = string("Client(") + std::to_string(client.isOperator()) + std::to_string(client.getFD()) + ", " + client.getNickname() + ", " + client.getUsername() + ", " + client.getHostname() + ", " + client.getServername() + ", " + client.getRealname() + ")";
	os << str;
	return os;
}

bool operator==(const Client &lhs, const Client &rhs)
{
	return 	lhs.getFD() == rhs.getFD() &&
			lhs.getNickname() == rhs.getNickname() &&
			lhs.getUsername() == rhs.getUsername();
}
