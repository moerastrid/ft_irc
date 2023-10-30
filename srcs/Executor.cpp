#include "Executor.hpp"

// some utilities #TODO move

#include <iostream>
using std::cout;
using std::endl;

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

//Executor::Executor() {
//	this->funcMap["CAP"] 		= &Executor::run_CAP;
//	this->funcMap["NICK"] 		= &Executor::run_NICK;
//	this->funcMap["USER"] 		= &Executor::run_USER;
//	this->funcMap["MODE"] 		= &Executor::run_MODE;
//	this->funcMap["PING"] 		= &Executor::run_PING;
//	this->funcMap["PRIVMSG"] 	= &Executor::run_PRIVMSG;
//	this->funcMap["WHOIS"] 		= &Executor::run_WHOIS;
//	this->funcMap["JOIN"] 		= &Executor::run_JOIN;
//	this->funcMap["KICK"] 		= &Executor::run_KICK;
//	this->funcMap["QUIT"] 		= &Executor::run_QUIT;
//	this->funcMap["PART"] 		= &Executor::run_PART;
//}

Executor::Executor(env& e) : e(e) {
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
	cout << "Sending message to fd " << fd << ":" << endl;
	cout << message << endl;
	const char* c_message = message.c_str();
	send(fd, c_message, sizeof c_message, 0);
}

string Executor::run(Command& cmd, int fd) {
	// Client* client = getClientByFD(fd); //Will not work until clients are added before this function is called.
	// if (client == NULL)
	// 	return "USER NOT FOUND\n"; // HELP

	string command = cmd.getCommand();
	mbrFuncPtr ptr;
	try {
		ptr = this->funcMap.at(command);
	}
	catch (std::out_of_range& e) {
		return build_reply(ERR_UNKNOWNCOMMAND, command, command, "Unknown command from client");
	}
	string message = (this->*ptr)(cmd.getArgs(), fd);

	// Prints all channels and a list of their connected clients.
	// for (auto& ch : this->e.channels) {
	// 	vector<Client> clients = ch.getClients();
	// 	cout << endl << ch.getName() << ": " << endl;
	// 	for (auto& client : clients) {
	// 		cout << client << endl;
	// 	}
	// 	cout << endl;
	// }

	return message;
}

string Executor::run_CAP([[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return this->e.server_address + " CAP NAK :-\n";
}

string Executor::run_NICK(vector<string> args, int fd) {
	if (args.size() != 1) {
		return build_reply(ERR_NEEDMOREPARAMS, "NICK", "NICK", "Not enough parameters");
	}

	string& nickname = args[0];

	if (nickname.empty() || !verify_name(nickname)) {
		return build_reply(ERR_ERRONEOUSNICKNAME, "NICK", nickname, "Erroneous nickname");
	}

	Client* caller = getClientByFD(fd);
	string message;

	if (caller != NULL) {
		if (caller->getNickname().empty()) { //first time connection part 2: electric boogaloo. Accepting connection and sending welcome message.
			message = build_reply(RPL_WELCOME, nickname, nickname, "Welcome to Astrid's & Thibauld's IRC server, " + caller->getUsername() + "!");
		} else {
			message = build_reply(RPL_WELCOME, nickname, nickname, "Nickname changed to " + nickname);
		}
		caller->setNickname(nickname);
	} else { //first time connection.
		addClient(nickname, "", "", "", "", fd);
		message = build_reply(NOTICE, nickname, nickname, "Remember to set your username using the USER command");
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

string Executor::run_USER(vector<string> args, int fd) {
	if (args.size() < 4) {
		return build_reply(ERR_NEEDMOREPARAMS, "USER", "USER", "Not enough parameters");
	}

	Client* client = getClientByFD(fd);

	string username, hostname, servername, realname;
	if (!parseUserArguments(args, username, hostname, servername, realname)) {
		return build_reply(ERR_ERRONEOUSNICKNAME, client->getNickname(), "USER", "Invalid user arguments");
	}

	string message;

	if (client != NULL) { //Existing connection.
		if (client->getUsername().empty()) {// first time connec part 2
			message = build_reply(RPL_WELCOME, client->getNickname(), username, "Welcome to Astrid's & Thibauld's IRC server, " + username + "!");
		} else {
			message = build_reply(RPL_WELCOME, client->getNickname(), username, "Username changed to " + username);
		}
		client->setUsername(username);
		client->setHostname(hostname);
		client->setServername(servername);
		client->setRealname(realname);
	}
	else { //first time connection.
		addClient("", username, hostname, servername, realname, fd);
		message = build_reply(NOTICE, client->getNickname(), username, "Remember to set your nickname using the NICK command");
	}
	return message;
}

string Executor::run_MODE(vector<string> args, int fd) {
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return build_reply(ERR_NEEDMOREPARAMS, "MODE", "MODE", "Not enough parameters");
	
	string target = args[0];
	string mode = args[1];

	Client* caller = getClientByFD(fd);
	if (caller == NULL) {
		return "CALLER NOT FOUND";
	}

	Channel* channel = getChannelByName(target);

	if (channel == NULL) {
		return build_reply(ERR_NOSUCHCHANNEL, caller->getNickname(), target, "No such channel");
	}

	string message = "";

	if (target[0] == '#') { // target is a channel.
		if (mode == "+i") {

		}
	} else {
		message = build_reply(ERR_UNKNOWNMODE, caller->getNickname(), mode, "Unknown mode");
	}

	return message;
}

string Executor::run_PING([[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return "PONG " + this->e.server_address;
}

string Executor::run_PRIVMSG(vector<string> args, int fd) {
	if (args.size() < 2 || args[0].empty() || args[1].empty()) {
		return build_reply(ERR_NEEDMOREPARAMS, "PRIVMSG", "PRIVMSG", "Not enough parameters");
	}
	Client* client = getClientByFD(fd);
	if (client == NULL) {
		return "USER NOT FOUND";
	}

	string message;
	Client* recipient = getClientByNickname(args[0]);
	if (recipient == NULL) {
		return build_reply(ERR_NOSUCHNICK, client->getNickname(), args[0], "No such recipient");
	}

	stringstream ss;

	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		ss << *it;
		if (std::next(it) != args.end())
			ss << " ";
	}
	ss << "\n";

	send_to_client(recipient->getFD(), ss.str());
	return "";
}

// #TODO finish
string Executor::run_WHOIS(vector<string> args, int fd) {
	// Any number of parameters is valid.
	Client* caller = getClientByFD(fd);
	if (caller == NULL) {
		return "CALLER NOT FOUND\n";
	}

	if (args.empty() || args[0].empty()) {
		string userinfo = caller->getNickname() + " " + caller->getUsername() + " " + caller->getHostname() + " * " + caller->getRealname();
		return build_reply(RPL_WHOISUSER, caller->getNickname(), caller->getNickname(), userinfo);
		// return ":" + this->e.server_address + " 311 " + caller->getNickname() + " " + caller->getNickname() + " " +  caller->getUsername() + " " + caller->getHostname() + " * :" + caller->getRealname() + "\n";
	}

	string finalmessage = "";

	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		string message = ":" + this->e.server_address + " 311 " + caller->getNickname() + " ";

		Client* requestee = getClientByNickname(*it);
		if (requestee == NULL) {
			// message = ":" + this->e.server_address + " 401 " + *it + " :No such nickname\n";
			message = build_reply(ERR_NOSUCHNICK, caller->getNickname(), *it, "No such nickname");
		} else {
			message += requestee->getNickname() + " " + requestee->getUsername() + " " + requestee->getHostname() + " * :" + requestee->getRealname() + "\n"; //#TODO fix servername? (servername == *)
		}
		finalmessage += message;
	}

	return finalmessage;
}

/*
 * incoming message: JOIN [<channel> | <password>]+
 */
string Executor::run_JOIN(vector<string> args, int fd) {
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

	Client* client = getClientByFD(fd);
	if (client == NULL) { // shit... this shouldn't happen. It means there is no registered client for the user sending the command.
		vector<string> quit_args = {"Screw you guys, I'm going home!"};
		return run_QUIT(quit_args, fd);
	}

	string finalmessage;
	for (const auto& pair : joininfo) {
		string message;
		const string& channelname = pair.first;
		const string& channelpassword = pair.second;
		Channel* ch = getChannelByName(channelname);
		message = ":" + this->e.server_address + " " + client->getNickname() + " JOIN " + channelname + "\n";
		if (ch == NULL) { // Create new channel and have user join as the first member.
			addChannel(channelname, channelpassword, client);
		} else {
			const vector<Client>& clients = ch->getClients();
			if (find(clients.begin(), clients.end(), *client) != clients.end())
				message = ": " + e.server_address + " 479 " + client->getNickname() + " " + ch->getName() + " :Cannot join channel - you are already on the channel\n";
			else if (ch->getPassword().compare(channelpassword) == 0)
				ch->addClient(*client);
			else //incorrect password.
				message = ":" + e.server_address + " 475 " + client->getNickname() + " " + channelname + " :Bad channel key\n";
		}
		finalmessage += message;
	}
	return finalmessage;
}

// Operator client wants to kick a user from a channel. #TODO check that the caller has the correct mode.
string Executor::run_KICK(vector<string> args, int fd) {
	if (args.size() < 2 || !args[0].size() || !args[1].size())
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";

	Client* caller = getClientByFD(fd);
	if (caller == NULL) { // This shouldn't happen yo.
		vector<string> quit_args = {"Screw you guys, I'm going home!"};
		return run_QUIT(quit_args, fd);
	}

	string message;
	Channel* ch = getChannelByName(args[0]);
	if (ch == NULL) {
		return ":" + this->e.server_address + " 403 " + args[0] + " :No such channel\n";
	}

	vector<string>::iterator reason_start = std::find_if(args.begin(), args.end(), [](std::string& str) {
		return !str.empty() && str[0] == ':';
	});

	for (vector<string>::iterator name_it = std::next(args.begin()); name_it != reason_start; name_it++) {
		Client* client = getClientByNickname(*name_it);
		if (client == NULL) {
			message += ":" + this->e.server_address + " 401 " + *name_it + " :No such nickname\n";
			continue;
		}

		else if (ch->removeClient(*client) == 1) {
			message += ":" + this->e.server_address + " 404 " + args[0] + " " + *name_it + " :Cannot kick user from channel they have not joined\n";
			continue;
		}

		message += ":" + this->e.server_address + " KICK " + args[0] + " " + *name_it;
		if (reason_start != args.end()) {
			message += " ";
			message += format_reason(reason_start, args);
		}
		message += "\n";
	}

	return message;
}

#include <iostream>
using std::cout;
using std::endl;

string Executor::run_PART(vector<string> args, int fd) {
	if (args.size() == 0 || args[0].empty()) {
		return "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n";
	}

	vector<string> channelnames = split_args(args[0]);

	Client* caller = getClientByFD(fd);
	if (caller == NULL) { // THis shouldn't happen yo.
		return "CALLER NOT FOUND\n";
	}

	vector<string>::iterator reason_start = std::find_if(args.begin(), args.end(), [](std::string& str) {
		return !str.empty() && str[0] == ':';
	});

	string message = "";
	for (vector<string>::iterator it = channelnames.begin(); it != channelnames.end(); it++) {
		Channel* ch = getChannelByName(*it);
		if (ch == NULL) {
			message += ":" + this->e.server_address + " 403 " + *it + " :No such channel\n";
			continue;
		}

		vector<Client> clients = ch->getClients();

		if(ch->removeClient(*caller) == 1) {
			message += ":" + this->e.server_address + " 442 " + *it + " :You haven't joined that channel";
			continue;
		}
		message += ":" + this->e.server_address + " PART " + *it + "\n";
		if (reason_start == args.end()) {
			cout << "skipping reason (#TODO change to default message)" << endl;
			continue;
		}

		// If there's a reason:
		// Send reason to all other users in channel, to inform them why the user left. //#TODO PROBABLY SHOULD USE SEND DIRECTLY TO FD INSTEAD OF SENDING A REPLY TO THE CLIENT.
		for (const Client& user : ch->getClients()) {
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

#include <unistd.h>

// Client wants to disconnect from server
string Executor::run_QUIT(vector<string> args, int fd) {
	(void)args;
	Client* client = getClientByFD(fd);
	if (client == NULL) {
		// close(fd);
		return "";
	}

	return "";
}

Client* Executor::getClientByFD(int fd) {
	vector<Client>& clients = this->e.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getFD() == fd)
			return &(*it);
	}

	return NULL;
}

Client* Executor::getClientByNickname(string nickname) {
	vector<Client>& clients = this->e.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nickname)
			return &(*it);
	}
	return NULL;
}

vector<Client>::iterator Executor::getItToClientByNickname(string nickname) {
	vector<Client>& clients = this->e.clients;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->getNickname() == nickname)
			return it;
	}
	return this->e.clients.end();
}

Channel* Executor::getChannelByName(string name) {
	for (Channel& ch : this->e.channels) {
		if (ch.getName() == name) {
			return &(ch);
		}
	}
	return NULL;
}

void Executor::addClient(string nickname, string username, string hostname, string servername, string realname, int fd) {
	Client new_client(fd);
	new_client.setNickname(nickname);
	new_client.setUsername(username);
	new_client.setHostname(hostname);
	new_client.setServername(servername);
	new_client.setRealname(realname);

	this->e.clients.push_back(new_client);
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

void Executor::addChannel(string name, string password, Client* client) {
	Channel c(name, password);
	c.addClient(*client);
	this->e.channels.push_back(c);
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


#include <iomanip>

string Executor::build_reply(int response_code, string callername, string target, string message) {
	if (response_code == NOTICE) {
		return build_notice_reply(target, callername, message);
	}
	stringstream ss;
	ss << std::setw(3) << std::setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.
	return ":" + this->e.server_address + " " + ss.str() + " " + callername + " " + target + " :" + message + "\n";
}

string Executor::build_notice_reply(string target, string callername, string message) {
	return ":" + this->e.server_address + " NOTICE " + callername + " " + target + " :" + message + "\n";
}