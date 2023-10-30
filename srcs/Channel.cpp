#include "Channel.hpp"

Channel::Channel() {
	this->topic = "";
	this->name = "";
	this->password = "";
};

Channel::~Channel() {};

Channel::Channel(const Channel& other) {
	this->topic = other.topic;
	this->name = other.name;
	this->password = other.password;
	this->clients = other.clients;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->topic = other.topic;
		this->name = other.name;
		this->password = other.password;
		this->clients = other.clients;
	}
	return *this;
}

Channel::Channel(string name, string password) {
	this->topic = "";
	this->name = name;
	this->password = password;
}

const string& Channel::getTopic() const {
	return this->topic;
}

const string& Channel::getName() const {
	return this->name;
}

const string& Channel::getPassword() const {
	return this->password;
}

const vector<Client>& Channel::getClients() const {
	return this->clients;
}

void Channel::setTopic(string& topic) {
	this->topic = topic;
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
