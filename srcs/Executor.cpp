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

int is_valid_nickname_character(char c) {
	if (isalnum(c) || c == '_' || c == '-' || c == '[' || c == ']' || c == '\\' || c == '^' || c == '{' || c == '}')
		return 1;
	return 0;
}

int verify_name(string arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_valid_nickname_character(*it))
			return false;
	}
	return true;
}

int verify_realname(string arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_valid_nickname_character(*it) && !std::isspace(*it))
			return false;
	}
	return true;
}

string Executor::run_NICK(env& env, vector<string> args, int fd) {
	if (args.size() != 1) {
		return "461 ERR_NEEDMOREPARAMS NICK :Not enough parameters";
	}

	string nickname = args[0];

	if (nickname.empty() || !verify_name(nickname)) {
		return "432 ERR_ERRONEOUSNICKNAME :Erroneous nickname";
	}

	Client* client = get_client_by_fd(env, fd);
	string message;

	if (client != NULL) {
		client->nickname = nickname;
		if (client->username.empty()) { //first time connection part 2: electric boogaloo. Accepting connection and sending welcome message.
			message = env.server_address + " 001 " + nickname + " :Welcome to Astrid's & Thibauld's IRC server, " + client->username + "!";
			return message;
		}
		// send normal nickname change message.
		message = client->nickname + " NICK :" + nickname;
	} else { //first time connection.
		Client new_client;
		new_client.fd = fd;
		new_client.nickname = nickname;
		new_client.username = "";
		message = ":" + env.server_address + " NOTICE " + nickname + ":Please set your username using the USER command.";
		env.clients.push_back(new_client);
	}
	return message;
}
/*
RFC 1459
4.1.3 User message

      Command: USER
   Parameters: <username> <hostname|ignored> <servername|ignored> <realname>

   The USER message is used at the beginning of connection to specify
   the username, hostname, servername and realname of s new user.  It is
   also used in communication between servers to indicate new user
   arriving on IRC, since only after both USER and NICK have been
   received from a client does a user become registered.

   Between servers USER must to be prefixed with client's NICKname.
   Note that hostname and servername are normally ignored by the IRC
   server when the USER command comes from a directly connected client
   (for security reasons), but they are used in server to server
   communication.  This means that a NICK must always be sent to a
   remote server when a new user is being introduced to the rest of the
   network before the accompanying USER is sent.

   It must be noted that realname parameter must be the last parameter,
   because it may contain space characters and must be prefixed with a
   colon (':') to make sure this is recognised as such.

   Since it is easy for a client to lie about its username by relying
   solely on the USER message, the use of an "Identity Server" is
   recommended.  If the host which a user connects from has such a
   server enabled the username is set to that as in the reply from the
   "Identity Server".
*/
string Executor::run_USER(env& env, vector<string> args, int fd) {
	if (args.size() < 4) {
		return "461 ERR_NEEDMOREPARAMS NICK :Not enough parameters";
	}
	if (args.size() > 4) {
		return "461 ERR_TOOMANYPARAMS NICK :Too many parameters";
	}

	string username = args[0];
	string hostname = args[1];
	string servername = args[2];
	string realname = args[3];

	if (username.empty() || !verify_name(username)) {
		return "432 ERR_ERRONEOUSNICKNAME :username cannot be empty.";
	}
	if (hostname.empty()) {
		return "432 ERR_ERRONEOUSNICKNAME :hostname cannot be empty.";
	}
	if (servername.empty()) {
		return "432 ERR_ERRONEOUSNICKNAME :servername cannot be empty.";
	}
	if (realname.empty() || args[3].compare(":") == 0) {
		return "432 ERR_ERRONEOUSNICKNAME :realname cannot be empty.";
	}

	Client* client = get_client_by_fd(env, fd);
	string message;

	if (client != NULL) { //Existing connection.
		client->username = username;
		if (client->nickname.empty()) {// first time connec part 2
			message = env.server_address + " 001 " + client->nickname + " :Welcome to Astrid's & Thibauld's IRC server, " + username + "!";
			return message;
		}
		message = ":" + env.server_address + " 482 " + username + ": Your username has been updated to " + username;
	}
	else { //first time connection.
		Client new_client;
		new_client.fd = fd;
		new_client.username = username;
		new_client.nickname = "";
		message = ":" + env.server_address + " NOTICE " + username + ":Please set your nickname using the NICK command.";
		env.clients.push_back(new_client);
	}
	return message;
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

Client* Executor::get_client_by_fd(env& env, int fd) {
	vector<Client>& clients = env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->fd == fd)
			return &(*it);
	}

	return NULL;
}
