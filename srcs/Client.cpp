#include "Client.hpp"

#include <iostream>
using std::cout;
using std::endl;

Client::Client() {
	this->fd = -1;
}

Client::~Client() {}

Client::Client(const Client& other) {
	this->fd = other.fd;
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->fd = other.fd;
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
	}
	return *this;
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
	string str = string("Client(") + std::to_string(client.fd) + ", " + client.nickname + ", " + client.username + ", " + client.hostname + ", " + client.servername + ", " + client.realname + ")";
	os << str;
	return os;
}