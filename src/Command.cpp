/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 13:29:45 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 13:29:46 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

// private
void Command::addArg(string arg) {
	this->args.push_back(arg);
}
void Command::setCmd(string cmd) {
	this->command = cmd;
}

// public
Command::Command() {
	Msg("command - default constructor", "CLASS");
}

Command::~Command() {
	Msg("command - default destructor", "CLASS");
}

Command::Command(const Command& other) {
	*this = other;
	Msg("command - copy constructor", "CLASS");
}

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
	Msg("command - constructor(const string& client_message_string)", "CLASS");
}

const Command& Command::operator=(const Command& other) {
	if (this != &other) {
		this->command = other.command;
		this->args = other.args;
	}
	Msg("command - assignment operator", "CLASS");
	return *this;
}

const string Command::getCommand() const {
	return this->command;
}
const vector<string> Command::getArgs() const {
	return this->args;
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

bool find_reason(const string& str) {
	auto it = str.begin();

	while (it != str.end() && isspace(*it))
		it++;

	return (it != str.end() && *it == ':');
}