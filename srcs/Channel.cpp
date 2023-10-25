#include "Channel.hpp"

Channel::Channel() {};

Channel::~Channel() {};

Channel::Channel(const Channel& other) {
	this->topic = other.topic;
	this->joined = other.joined;
	this->operators = other.operators;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {

	}
	return *this;
}
