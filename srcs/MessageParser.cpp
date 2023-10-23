#include "MessageParser.hpp"

MessageParser::MessageParser() {

}

MessageParser::~MessageParser() {}


Command MessageParser::parse_client_message(void* env, const string& client_message_string, const int active_client_fd) {
	Command cmd;
	stringstream ss(client_message_string);

	string cmd_str;
	ss >> cmd_str;
	cmd.setCmd(cmd_str);

	std::string arg;
	while (ss >> arg) {
		cmd.addArg(arg);
	}

	return cmd;
}

int validate_command(Command& cmd) {
	vector<string> valid_commands = {"CAP", "NICK"};// "USER", "MODE", "PING", "PRIVMSG", "WHOIS", "JOIN", "KICK", "QUIT" };

	if (std::find(valid_commands.begin(), valid_commands.end(),  cmd.getCommand()) == valid_commands.end()) {
		//send ERR_UNKNOWNCOMMAND as ["421 ERR_UNKNOWNCOMMAND :"human-readable error message"]
		return -1;
	}
	return 0;
}

#include <iostream>
using std::cout;
using std::endl;

int main() {
	MessageParser mp;
	int fd = 4;
	Command cmd = mp.parse_client_message(NULL, "NICK thibauld 123 test \n", fd);

	cout << cmd.getCommand() << endl;
	for (auto& arg : cmd.getArgs()) {
		cout << arg << endl;
	}
}