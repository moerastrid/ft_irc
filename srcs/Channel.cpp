#include "Channel.hpp"

Channel::Channel() {
	this->name = "";
	this->password = "";
	this->topic = "";
	this->inviteOnly = false;
	this->userLimit = 0;
};

Channel::~Channel() {};

Channel::Channel(const Channel& other) {
	this->name = other.name;
	this->password = other.password;
	this->topic = other.topic;
	this->clients = other.clients;
	this->inviteOnly = other.inviteOnly;
	this->userLimit = other.userLimit;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->name = other.name;
		this->password = other.password;
		this->topic = other.topic;
		this->clients = other.clients;
		this->inviteOnly = other.inviteOnly;
		this->userLimit = other.userLimit;
	}
	return *this;
}

Channel::Channel(string name, string password) {
	this->name = name;
	this->password = password;
	this->topic = "Welcome to channel " + name;
	this->inviteOnly = false;
	this->userLimit = 0;
}


const string& Channel::getName() const {
	return this->name;
}
const string& Channel::getPassword() const {
	return this->password;
}
const string& Channel::getTopic() const {
	return this->topic;
}
bool Channel::getInviteStatus() const {
	return this->inviteOnly;
}
const vector<Client>& Channel::getClients() const {
	return this->clients;
}
size_t Channel::getUserLimit() const {
	return this->userLimit;
}

void Channel::setTopic(string& topic) {
	this->topic = topic;
}
void Channel::makeInviteOnly() {
	this->inviteOnly = true;
}
void Channel::takeInviteOnly() {
	this->inviteOnly = false;
}
void Channel::setUserLimit(size_t limit) {
	this->userLimit = limit;
}

#include <iostream>

int Channel::removeClient(Client client) {
	auto it = std::find(clients.begin(), clients.end(), client);
	if (it == clients.end()) {
		return 1;
	}
	this->clients.erase(it);
	return 0;
}

void Channel::addClient(Client client) {
	if (find(this->clients.begin(), this->clients.end(), client) == this->clients.end()) {
		this->clients.push_back(client);
	}
}

std::ostream& operator<<(std::ostream& os, const Channel& channel) {
	os << "Channel(" << channel.getName() << ", [";
	const vector<Client>& clients = channel.getClients();
	for (auto it = clients.begin(); it != clients.end(); it++) {
		os << *it;
		if (it + 1 != clients.end()) {
			os << ", ";
		}
	}
	os << "])";

	return os;
}

bool operator==(const Channel& lhs, const Channel& rhs) {
	return lhs.getName() == rhs.getName();
}
