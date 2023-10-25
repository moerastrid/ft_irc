#include "Command.hpp"

Command::Command(const string& client_message_string, int client_fd) {
	stringstream ss(client_message_string);

	string cmd_str;
	ss >> cmd_str;
	this->setCmd(cmd_str);

	std::string arg;
	while (ss >> arg) {
		this->addArg(arg);
	}

	this->client_fd = client_fd;
}

Command::Command() {
	this->client_fd = -1;
}

Command::~Command() {}

const Command& Command::operator=(const Command& other) {
	if (this != &other) {
		this->command = other.command;
		this->args = other.args;
	}
	return *this;
}

void Command::setCmd(string cmd) {
	this->command = cmd;
}

void Command::addArg(string arg) {
	this->args.push_back(arg);
}

string Command::getCommand() {
	return this->command;
}

//throws exception if idx too large
string Command::getArg(size_t idx) {
	return this->args.at(idx);
}

vector<string> Command::getArgs() {
	return this->args;
}
