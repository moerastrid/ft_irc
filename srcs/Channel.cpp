#include "Channel.hpp"

Channel::Channel() {};

Channel::~Channel() {};

Channel::Channel(const Channel& other) {
	this->topic = other.topic;
	this->name = other.name;
	this->password = other.password;
	this->joined = other.joined;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->topic = other.topic;
		this->name = other.name;
		this->password = other.password;
		this->joined = other.joined;
	}
	return *this;
}
