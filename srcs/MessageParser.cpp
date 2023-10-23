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