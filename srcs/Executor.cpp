#include "Executor.hpp"

Executor::Executor() {
	// this->valid_commands.push_back("CAP");
	// this->valid_commands.push_back("NICK");
	// this->valid_commands.push_back("USER");
	// this->valid_commands.push_back("MODE");
	// this->valid_commands.push_back("PING");
	// this->valid_commands.push_back("PRIVMSG");
	// this->valid_commands.push_back("WHOIS");
	// this->valid_commands.push_back("JOIN");
	// this->valid_commands.push_back("KICK");
	// this->valid_commands.push_back("QUIT");

	this->funcMap["CAP"] = &Executor::run_CAP;
	this->funcMap["NICK"] = &Executor::run_NICK;
	this->funcMap["USER"] = &Executor::run_USER;
	this->funcMap["MODE"] = &Executor::run_MODE;
	this->funcMap["PING"] = &Executor::run_PING;
	this->funcMap["PRIVMSG"] = &Executor::run_PRIVMSG;
	this->funcMap["WHOIS"] = &Executor::run_WHOIS;
	this->funcMap["JOIN"] = &Executor::run_JOIN;
	this->funcMap["QUIT"] = &Executor::run_QUIT;
}

Executor::~Executor() {}

void Executor::send_to_client(int fd, string message) {
	const char* c_message = message.c_str();
	send(fd, c_message, sizeof c_message, 0);
}

void Executor::run(void* env, Command& cmd, int fd) {
	string command = cmd.getCommand();
	mbrFuncPtr ptr;
	try {
		ptr = this->funcMap.at(command);
	}
	catch (std::out_of_range e) {
		//send ERR_UNKNOWNCOMMAND as ["421 ERR_UNKNOWNCOMMAND :"human-readable error message"]
		send_to_client(fd, "421 ERR_UNKNOWNCOMMAND :Unknown command from client");
	}
	string message = (this->*ptr)(env, cmd.getArgs());
	send_to_client(fd, message);
}

string Executor::run_CAP(void* env, vector<string> args) {
	return "";
}

string Executor::run_NICK(void* env, vector<string> args) {
	return "";
}

string Executor::run_USER(void* env, vector<string> args) {
	return "";
}

string Executor::run_MODE(void* env, vector<string> args) {
	return "";
}

string Executor::run_PING(void* env, vector<string> args) {
	return "";
}

string Executor::run_PRIVMSG(void* env, vector<string> args) {
	return "";
}

string Executor::run_WHOIS(void* env, vector<string> args) {
	return "";
}

string Executor::run_JOIN(void* env, vector<string> args) {
	return "";
}

string Executor::run_KICK(void* env, vector<string> args) {
	return "";
}

string Executor::run_QUIT(void* env, vector<string> args) {
	return "";
}
