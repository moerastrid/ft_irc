#include "Command.hpp"

Command::Command(string cmd, vector<string> args) {
	this->command = cmd;
	this->args = args;
}

Command::Command() {}

Command::~Command() {}

const Command& Command::operator=(const Command& other) {
	if(this != &other) {
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
