#include "Client.hpp"

Client::Client() {
	this->fd = -1;
}

Client::~Client() {}

Client::Client(const Client& other) {
	this->fd = other.fd;
	this->username = other.username;
	this->realname = other.realname;
	this->nickname = other.nickname;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->fd = other.fd;
		this->username = other.username;
		this->realname = other.realname;
		this->nickname = other.nickname;
	}
	return *this;
}

// Client::Client(/*args go here*/) {

// }
