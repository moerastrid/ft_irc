#include "Command.hpp"

Command::Command(const string& client_message_string) {
	stringstream ss(client_message_string);

	string cmd_str;
	ss >> cmd_str;
	this->setCmd(cmd_str);

	string arg;
	while (ss >> arg) {
		this->addArg(arg);
	}

	this->combine_reason();

	// this->client_fd = client_fd;
}

Command::Command() {
	// this->client_fd = -1;
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

bool find_reason(const string& str) {
	auto it = str.begin();

	while (it != str.end() && isspace(*it))
		it++;

	return (it != str.end() && *it == ':');
}

void Command::combine_reason() {
	vector<string>::iterator it = find_if(this->args.begin(), this->args.end(), find_reason);
	if (it == args.end())
		return ;

	vector<string>::iterator itc = it;

	string newstring;
	while (itc != this->args.end()) {
		newstring += *itc;
		itc++;
		if (itc != args.end())
			newstring += " ";
	}

	args.erase(it, args.end());
	args.push_back(newstring);
}