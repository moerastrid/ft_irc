#include "Executor.hpp"

// some utilities #TODO move

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

// Takes a comma-separated string of arguments, gives back a vector of said arguments.
vector<string> split_args(string args) {
	std::istringstream nameStream(args);
	std::vector<std::string> res;

	std::string buffer;
	while (std::getline(nameStream, buffer, ',')) {
		res.push_back(buffer);
	}

	return res;
}

// Executor class

Executor::Executor() {
	this->funcMap["CAP"] 		= &Executor::run_CAP;
	this->funcMap["NICK"] 		= &Executor::run_NICK;
	this->funcMap["USER"] 		= &Executor::run_USER;
	this->funcMap["MODE"] 		= &Executor::run_MODE;
	this->funcMap["PING"] 		= &Executor::run_PING;
	this->funcMap["PRIVMSG"] 	= &Executor::run_PRIVMSG;
	this->funcMap["WHOIS"] 		= &Executor::run_WHOIS;
	this->funcMap["JOIN"] 		= &Executor::run_JOIN;
	this->funcMap["KICK"] 		= &Executor::run_KICK;
	this->funcMap["QUIT"] 		= &Executor::run_QUIT;
	this->funcMap["PART"] 		= &Executor::run_PART;
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
		return "421 ERR_UNKNOWNCOMMAND :Unknown command from client\n";
	}
	string message = (this->*ptr)(env, cmd.getArgs(), fd);
	return message;
}

string Executor::run_CAP(env& env, [[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return env.server_address + " CAP NAK :-\n";
}

string Executor::run_NICK(env& env, vector<string> args, int fd) {
	if (args.size() != 1) {
		return "461 ERR_NEEDMOREPARAMS NICK :Not enough parameters\n";
	}

	string& nickname = args[0];

	if (nickname.empty() || !verify_name(nickname)) {
		return "432 ERR_ERRONEOUSNICKNAME :Erroneous nickname\n";
	}

	Client* caller = getClientByFD(env, fd);
	string message;

	if (caller != NULL) {
		if (caller->getNickname().empty()) { //first time connection part 2: electric boogaloo. Accepting connection and sending welcome message.
			message = env.server_address + " 001 " + nickname + " :Welcome to Astrid's & Thibauld's IRC server, " + caller->getUsername() + "!\n";
		} else {			// send normal nickname change message.
			message = caller->getNickname() + " NICK :" + nickname + "\n";
		}
		caller->setNickname(nickname);
	} else { //first time connection.
		addClient(env, nickname, "", "", "", "", fd);
		message = ":" + env.server_address + " NOTICE " + nickname + ":Remember to set your username using the USER command.\n";
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
		return "461 ERR_NEEDMOREPARAMS USER :Not enough parameters\n";
	}

	string username, hostname, servername, realname;
	if (!parseUserArguments(args, username, hostname, servername, realname)) {
		return "432 ERR_ERRONEOUSNICKNAME :Invalid user arguments\n";
	}

	Client* client = getClientByFD(env, fd);
	string message;

	if (client != NULL) { //Existing connection.
		if (client->getUsername().empty()) {// first time connec part 2
			message = ":" + env.server_address + " 001 " + client->getNickname() + " :Welcome to Astrid's & Thibauld's IRC server, " + username + "!\n";
		} else {
			message = ":" + env.server_address + " 482 " + username + ": Your username has been updated to " + username + "\n";
		}
		client->setUsername(username);
		client->setHostname(hostname);
		client->setServername(servername);
		client->setRealname(realname);
	}
	else { //first time connection.
		addClient(env, "", username, hostname, servername, realname, fd);
		message = ":" + env.server_address + " NOTICE " + username + ":Remember to set your nickname using the NICK command.\n";
	}
	return message;
}

string Executor::run_MODE(env& env, vector<string> args, int fd) {
	if (args.size() < 2)
		return "461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n";
	
	string target = args[0];
	string mode = args[1];
	if (target.empty() || mode.empty()) {
		return "461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n";
	}

	string message = "";
	Client* caller = getClientByFD(env, fd);

	if (target[0] == '#') { // target is a channel.
		if (mode == "+i") {

		}
	} else {
		message = ":" + env.server_address + " 472 " + caller->getNickname() + " " + mode + ":Unknown MODE.\n";
	}

	message = ":" + env.server_address + " 472 " + caller->getNickname() + " " + mode + ":Unknown MODE.\n"; //temp
	return message;
}

string Executor::run_PING(env& env, [[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return "PONG " + env.server_address;
}

string Executor::run_PRIVMSG(env& env, vector<string> args, int fd) {
	(void)env;
	(void)args;
	(void)fd;
	return "";
}

// #TODO finish
string Executor::run_WHOIS(env& env, vector<string> args, int fd) {
	// Any number of parameters is valid.

	vector<Client> clients = env.clients;
	Client* caller = getClientByFD(env, fd);

	string finalmessage = "";

	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		string message = ":" + env.server_address + " 311 " + caller->getNickname() + " ";
		Client* requestee = getClientByNickname(env, *it);
		if (requestee == NULL) {
			message = ":" + env.server_address + " 401 " + *it + " :No such nickname\n";
		} else {
			message += requestee->getNickname() + " " + requestee->getUsername() + " " + requestee->getHostname() + " * :" + requestee->getRealname() + "\n";
		}
		finalmessage += message;
	}

	return finalmessage;
}

/*
 * incoming message: JOIN [<channel> | <password>]+
 */
string Executor::run_JOIN(env& env, vector<string> args, int fd) {
	if (args.size() == 0) {
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";
	}
	if (args.size() > 2) {
		return "461 ERR_TOOMANYPARAMS JOIN :Too many parameters\n";
	}
	if (args[0].empty()) {
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";
	}
	if (args[0][0] == '0') { // /JOIN 0 = leave all joined channels.
		//leave all channels;
		return "";
	}

	string pwds = "";
	if (args.size() == 2) {
		pwds = args[1];
	}

	map<string, string> joininfo;
	vector<string> channelnames = split_args(args[0]);
	vector<string> channelpasswords = split_args(pwds);
	for (size_t i = 0; i < channelnames.size(); i++) {
		string channelpassword = "";
		if (i < channelpasswords.size()) {
			channelpassword = channelpasswords[i];
		}
		joininfo[channelnames[i]] = channelpassword;
	}

	Client* client = getClientByFD(env, fd);
	if (client == NULL) { // shit... this shouldn't happen. It means there is no registered client for the user sending the command.
		vector<string> quit_args = {"Screw you guys, I'm going home!"};
		return run_QUIT(env, quit_args, fd);
	}

	string message = "";
	for (const auto& pair : joininfo) {
		const string& channelname = pair.first;
		const string& channelpassword = pair.second;
		Channel* ch = getChannelByName(env, channelname);
		message += ":" + env.server_address + " " + client->getNickname() + " JOIN " + channelname + "\n";
		if (ch == NULL) { // Create new channel and have user join as the first member.
			addChannel(env, channelname, channelpassword, client);
		} else {
			if (ch->getPassword().compare(channelpassword) == 0) {
				ch->addClient(*client);
			} else { //incorrect password.
				message += ":" + env.server_address + " 475 " + client->getNickname() + " " + channelname + ":Bad channel key\n";
			}
		}
	}


	return message;
}

// Operator client wants to kick a user from a channel. #TODO check that the caller has the correct mode.
string Executor::run_KICK(env& env, vector<string> args, int fd) {
	if (args.size() < 2 || !args[0].size() || !args[1].size())
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";

	Client* caller = getClientByFD(env, fd);
	if (caller == NULL) { // This shouldn't happen yo.
		vector<string> quit_args = {"Screw you guys, I'm going home!"};
		return run_QUIT(env, quit_args, fd);
	}

	string message;
	Channel* ch = getChannelByName(env, args[0]);
	if (ch == NULL) {
		return ":" + env.server_address + " 403 " + args[0] + " :No such channel\n";
	}

	vector<string>::iterator reason_start = std::find_if(args.begin(), args.end(), [](std::string& str) {
		return !str.empty() && str[0] == ':';
	});

	for (vector<string>::iterator name_it = std::next(args.begin()); name_it != reason_start; name_it++) {
		Client* client = getClientByNickname(env, *name_it);
		if (client == NULL) {
			message += ":" + env.server_address + " 401 " + *name_it + " :No such nickname\n";
			continue;
		}

		else if (ch->removeClient(*client) == 1) {
			message += ":" + env.server_address + " 404 " + args[0] + " " + *name_it + ":Cannot kick user from channel they have not joined\n";
			continue;
		}

		message += ":" + env.server_address + " KICK " + args[0] + " " + *name_it;
		if (reason_start != args.end()) {
			message += " ";
			message += format_reason(reason_start, args);
		}
		message += "\n";
	}

	return message;
}

string Executor::run_PART(env& env, vector<string> args, int fd) {
	if (args.size() == 0) {
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";
	}

	Client* caller = getClientByFD(env, fd);
	if (caller == NULL) { // THis shouldn't happen yo.
		vector<string> quit_args = {"Screw you guys, I'm going home!"};
		return run_QUIT(env, quit_args, fd);
	}

	vector<string>::iterator reason_start = std::find_if(args.begin(), args.end(), [](std::string& str) {
		return !str.empty() && str[0] == ':';
	});

	string message = "";
	for (vector<string>::iterator it = args.begin(); it != reason_start; it++) {
		Channel* ch = getChannelByName(env, *it);
		if (ch == NULL) {
			message += ":" + env.server_address + " 403 " + *it + " :No such channel\n";
			continue;
		}

		if(ch->removeClient(*caller) == 1) {
			message += ":" + env.server_address + " 442 " + *it + " :You haven't joined that channel";
			continue;
		}
		message += ":" + env.server_address + " PART " + *it + "\n";
		if (reason_start == args.end()) {
			continue;
		}
		// If there's a reason:
		// Send reason to all other users in channel, to inform them why the user left. //#TODO PROBABLY SHOULD USE SEND DIRECTLY TO FD INSTEAD OF SENDING A REPLY TO THE CLIENT.
		for (const Client user : ch->getClients()) {
			string reasonmessage = "";
			reasonmessage += ":" + caller->getNickname() + "!" + caller->getUsername() + "@" + caller->getHostname() + " PRIVMSG " + user.getNickname();
			if (reason_start != args.end()) {
				reasonmessage += " ";
				reasonmessage += format_reason(reason_start, args);
			}
			reasonmessage += "\n";
			send_to_client(fd, reasonmessage);
		}
	}

	return message;
}


// Client wants to disconnect from server
string Executor::run_QUIT(env& env, vector<string> args, int fd) {
	(void)args;
	Client* client = getClientByFD(env, fd);
	if (client == NULL) {
		//Close the connection;
		return "";
	}



	return "";
}

Client* Executor::getClientByFD(env& env, int fd) {
	vector<Client>& clients = env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return &(*it);
	}

	return NULL;
}

Client* Executor::getClientByNickname(env& env, string nickname) {
	vector<Client>& clients = env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nickname)
			return &(*it);
	}
	return NULL;
}

vector<Client>::iterator Executor::getItToClientByNickname(env& env, string nickname) {
	vector<Client>& clients = env.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nickname)
			return it;
	}
	return env.clients.end();
}

Channel* Executor::getChannelByName(env& env, string name) {
	for (Channel& ch : env.channels) {
		if (ch.getName() == name) {
			return &(ch);
		}
	}
	return NULL;
}

void Executor::addClient(env& env, string nickname, string username, string hostname, string servername, string realname, int fd) {
	Client new_client(fd);
	new_client.setNickname(nickname);
	new_client.setUsername(username);
	new_client.setHostname(hostname);
	new_client.setServername(servername);
	new_client.setRealname(realname);

	env.clients.push_back(new_client);
}

bool Executor::parseUserArguments(const vector<string>& args, string& username, string& hostname, string& servername, string& realname) {
	username = args[0];
	hostname = args[1];
	servername = args[2];
	realname = args[3].substr(1);

	for (size_t i = 4; i < args.size(); i++) {
		realname += " " + args[i];
	}

	return !username.empty() && verify_name(username) && !hostname.empty() && !servername.empty() && (!realname.empty() || args[3] != ":");
}

void Executor::addChannel(env& env, string name, string password, Client* client) {
	Channel c(name, password);
	c.addClient(*client);
	env.channels.push_back(c);
}

string Executor::format_reason(vector<string>::iterator& reason_start, vector<string>& args) {
	string message = "";
	for (vector<string>::iterator it = reason_start; it != args.end(); it++) {
		message += *it;
		if (std::next(it) != args.end()) {
			message += " ";
		}
	}
	return message;
}
