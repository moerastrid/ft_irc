#include "Executor.hpp"

Executor::Executor() {
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

string Executor::run(env& env, Command& cmd, int fd) {
	string command = cmd.getCommand();
	mbrFuncPtr ptr;
	try {
		ptr = this->funcMap.at(command);
	}
	catch (std::out_of_range& e) {
		return "421 ERR_UNKNOWNCOMMAND :Unknown command from client";
	}
	string message = (this->*ptr)(env, cmd.getArgs(), fd);
	return message;
}

string Executor::run_CAP(env& env, vector<string> args, int fd) {
	return env.server_address + " CAP NAK :-";
}

string Executor::run_NICK(env& env, vector<string> args, int fd) {
	Client client;
	string message;

	if (args.size() == 0 || args[0].compare("") == 0) {
		return "432 ERR_ERRONEOUSNICKNAME :NICK cannot be empty.";
	}

	try {
		client = get_client_by_fd(env, fd);
		message = client.nickname + " NICK :" + args[0];
		client.nickname = args[0];
	}
	catch (const std::runtime_error& e) {
		client.fd = fd;
		client.nickname = args[0];
		message = env.server_address + " 001 " + client.nickname + " :Welcome to Astrid's & Thibauld's IRC server, " + client.nickname + "!";
		env.clients.push_back(client);
	}

	return message;
}

string Executor::run_USER(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_MODE(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_PING(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_PRIVMSG(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_WHOIS(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_JOIN(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_KICK(env& env, vector<string> args, int fd) {
	return "";
}

string Executor::run_QUIT(env& env, vector<string> args, int fd) {
	return "";
}

Client Executor::get_client_by_fd(env& env, int fd) {
	vector<Client>& clients = env.clients;
	for (vector<Client>::const_iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->fd == fd)
			return *it;
	}

	throw std::runtime_error("Client not found");
}
