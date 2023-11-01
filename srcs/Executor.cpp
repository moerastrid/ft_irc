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

Executor::Executor(env& e) : e(e) {
	this->funcMap["CAP"] 		= &Executor::run_CAP;
	this->funcMap["PASS"] 		= &Executor::run_PASS;
	this->funcMap["NICK"] 		= &Executor::run_NICK;
	this->funcMap["USER"] 		= &Executor::run_USER;
	this->funcMap["MODE"] 		= &Executor::run_MODE;
	this->funcMap["PING"] 		= &Executor::run_PING;
	this->funcMap["PRIVMSG"] 	= &Executor::run_PRIVMSG;
	this->funcMap["WHOIS"] 		= &Executor::run_WHOIS;
	this->funcMap["JOIN"] 		= &Executor::run_JOIN;
	this->funcMap["KICK"] 		= &Executor::run_KICK;
	this->funcMap["PART"] 		= &Executor::run_PART;
	this->funcMap["INVITE"] 	= &Executor::run_INVITE;
	this->funcMap["TOPIC"] 		= &Executor::run_TOPIC;
	this->funcMap["QUIT"] 		= &Executor::run_QUIT;

	this->argCount["CAP"] 		= {1, 1};
	this->argCount["PASS"] 		= {1, 1};
	this->argCount["NICK"] 		= {1, 1};
	this->argCount["USER"] 		= {4, -1};
	this->argCount["MODE"] 		= {1, 3};
	this->argCount["PING"] 		= {1, 1};
	this->argCount["PRIVMSG"] 	= {2, -1};
	this->argCount["WHOIS"] 	= {0, -1};
	this->argCount["JOIN"] 		= {1, 2};
	this->argCount["KICK"] 		= {1, -1};
	this->argCount["PART"] 		= {1, -1};
	this->argCount["INVITE"] 	= {1, 2};
	this->argCount["TOPIC"] 	= {1, -1};
	this->argCount["QUIT"] 		= {0, -1};
}

Executor::~Executor() {}

void Executor::send_to_client(int fd, string message) {
	cout << endl << "Sending [" << message << "] to client" << endl;
	const char* c_message = message.c_str();
	send(fd, c_message, sizeof c_message, 0);
}

int Executor::validateArguments(const std::string& command, int numArgs) {
	if (this->argCount.find(command) != this->argCount.end()) {
		std::pair<int, int> argCounts = this->argCount[command];
		int minArgs = argCounts.first;
		int maxArgs = argCounts.second;

		// Check if the number of arguments falls within the specified range.
		if (numArgs < minArgs) {
			return -1;
		}
		if (maxArgs >= 0 && numArgs > maxArgs) {
			return 1;
		}
	}

	return 0; // Argument count is valid.
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

	 int numArgs = cmd.getArgs().size();

	// Validate the number of arguments for the command.
	 //#TODO add client nickname.
	int res = validateArguments(command, numArgs);
	if (res == -1) {
		return build_reply(ERR_NEEDMOREPARAMS, command, command, "Not enough parameters");
	} else if (res == 1) {
		return build_reply(ERR_TOOMANYPARAMS, command, command, "Too many parameters");
	}

	string message = (this->*ptr)(cmd.getArgs(), fd);

	// // Prints all channels and a list of their connected clients.
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

/*
 * Incoming message: CAP LS
 * Possible replies: :<server=localhost> CAP NAK :<list of capabilities=->
 * 
 * Responses handled:
 * :<server=localhost> CAP NAK :-
 * 
 * Responses not (yet) handled:
 */
string Executor::run_CAP([[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return this->e.server_address + " CAP NAK :-\n";
}

/*
 * Incoming message: PASS <password> (not a user command, sent by the server if a password is supplied)
 * Possible replies: ???
 * 
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * 
 * Responses not (yet) handled:
 * ERR_ALREADYREGISTRED
 */
string Executor::run_PASS(vector<string> args, int fd) {
	(void)args;
	(void)fd;
	return "";
}

/*
 * Incoming message: NICK <nickname>
 * Parameters: Your new nickname.
 * Description: Changes your nickname on the active server.
 * Possible replies:
 * 
 * Responses handled:
 * ERR_NONICKNAMEGIVEN (override by ERR_NEEDMOREPARAMS... #TODO fix?)
 * ERR_ERRONEUSNICKNAME
 * 
 * Responses not (yet) handled:
 * ERR_NICKNAMEINUSE
 * ERR_NICKCOLLISION
 */
string Executor::run_NICK(vector<string> args, int fd) {
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

/*
 * Incoming message: USER <username> <hostname> <servername> :<realname> (Not a user command, but automatically sent by the client on a new connection.)
 * Possible replies:
 * 
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED
 * 
 * Responses not (yet) handled:
 * 
 */
string Executor::run_USER(vector<string> args, int fd) {
	Client* client = getClientByFD(fd);

	string username, hostname, servername, realname;
	if (!parseUserArguments(args, username, hostname, servername, realname)) {
		if (client == NULL || client->getNickname().empty()) {
			return build_reply(ERR_ERRONEOUSNICKNAME, "USER", "USER", "Invalid user arguments");
		}
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
		message = build_reply(NOTICE, username, username, "Remember to set your nickname using the NICK command");
	}
	return message;
}

/*
 * Incoming message: MODE <channel> [<mode> [<mode parameters>]]
 * Mode arguments: (set with +<mode> or -<mode>)
 * 		i				: invite only
 * 		t				: Topic change only by operator
 * 		k <key>			: Adds/removes password
 * 		o <nickname>	: Give <nickname> operator status.
 * 		l <count>		: Set max number of users for channel (0 = unlimited).
 * Parameters: The target nickname or channel and the modes with their parameters to set or remove. If the target nickname or channel is omitted, the active nickname or channel will be used.
 * Description: Modifies the channel modes for which you are privileged to modify. You can specify multiple modes in one command and prepend them by using the ‘+’ sign to set or ‘-’ sign to unset; modes that require a parameter will be retrieved from the argument list.
 * 
 * Possible replies: ???
 *
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * ERR_UNKNOWNMODE
 * ERR_NOSUCHCHANNEL
 *
 * Responses not yet handled:
 * RPL_CHANNELMODEIS (Response to mode status request) "<channel> <mode> <mode params>"
 * RPL_UMODEIS (- To answer a query about a client's own mode, RPL_UMODEIS is sent back.)
 * ERR_CHANOPRIVSNEEDED (User without operator status request mode change that requires OP)
 * ERR_NOSUCHNICK (Used to indicate the nickname parameter supplied to a command is currently unused.)
 * ERR_NOTONCHANNEL (Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 * ERR_KEYSET (User joins channel with wrong key. Not sent if no key is set for the channel)
 * ERR_USERSDONTMATCH (Error sent to any user trying to view or change the user mode for a user other than themselves.)
 * ERR_UMODEUNKNOWNFLAG (Returned by the server to indicate that a MODE message was sent with a nickname parameter and that the a mode flag sent was not recognized.)
 * 
 * Responses not handled:
 * RPL_BANLIST
 * RPL_ENDOFBANLIS
 */
string Executor::run_MODE(vector<string> args, int fd) {
	string target = args[0];
	string mode = args[1];

	Client* caller = getClientByFD(fd);
	if (caller == NULL) {
		return "USER NOT FOUND";
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

/*
 * Incoming message: PING {no args} (not a user command, just a reply to the automatic PING request from the server to indicate the connection is alive)
 * Possible replies: PONG <server=:localhost>
 * 
 * Responses handled:
 * PONG <server=:localhost>
 * 
 * Responses not (yet) handled:
 */
string Executor::run_PING([[maybe_unused]]vector<string> args, [[maybe_unused]]int fd) {
	return "PONG " + this->e.server_address + "\n";
}

/*
 * Incoming message: PRIVMSG [-channel | -nick] *|<targets> <message>
 * Parameters: The target nickname or channel and the message to send.
 * -channel: Indicates that the target or targets is a channel.
 * -nick: Indicates that the target or targets is a nickname.
 * The target can be a comma delimited list of targets, ie nick1,nick2 or #chan1,#chan2
 * Or one of the following special targets:
 * `*` : Use the active nickname or channel
 * `,` : Last person who sent you a /msg
 * `.` : Last person you sent a /msg to
 * Description: Sends a message to a nickname or channel.
 *
 * Possible replies: ???
 *
 * Responses handled:
 * ERR_NOSUCHNICK
 *
 * Responses not yet handled:
 * ERR_NORECIPIENT
 * ERR_NOTEXTTOSEND
 * ERR_CANNOTSENDTOCHAN
 * ERR_TOOMANYTARGETS
 * RPL_AWAY
 * 
 * Responses not handled:
 * ERR_NOTOPLEVEL
 * ERR_WILDTOPLEVEL
 */
string Executor::run_PRIVMSG(vector<string> args, int fd) {
	string target = args[0];

	Client* client = getClientByFD(fd);
	if (client == NULL) {
		return "USER NOT FOUND";
	}

	string message;
	Client* recipient = getClientByNickname(target);
	if (recipient == NULL) {
		return build_reply(ERR_NOSUCHNICK, client->getNickname(), target, "No such recipient");
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
/*
 * Incoming message: WHOIS [<nick>[,<nick>]*]
 * Parameters: The nicknames to query. If no nickname is given, you will query yourself.
 * Description: Displays information about users in the specified channel.
 *
 * Possible replies: ???
 *
 * Responses handled:
 * ERR_NOSUCHNICK (Used to indicate the nickname parameter supplied to a command is currently unused.)
 *
 * Responses not yet handled:
 * 
 * Responses not handled:
 * ERR_TOOMANYTARGETS 	(Returned to a client which is attempting to send a PRIVMSG/NOTICE using the user@host destination format and for a user@host which has several occurrences.)
 * RPL_AWAY 			(Sent as a reply to a PRIVMSG when the client has set themselves to AWAY)
 * ERR_CANNOTSENDTOCHAN (Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on a channel which has mode +m set and is trying to send a PRIVMSG message to that channel.)
 * ERR_NORECIPIENT 		(More specific than ERR_NEEDMOREPARAMS, but effectively identical on a server that doesn't do server-server communication)
 * ERR_NOTEXTTOSEND 	(More specific than ERR_NOSUCHNICK, but effectively identical)
 * ERR_NOTOPLEVEL		(412 - 414 are returned by PRIVMSG to indicate that the message wasn't delivered for some reason. ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that are returned when an invalid use of "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.)
 * ERR_WILDTOPLEVEL 	(412 - 414 are returned by PRIVMSG to indicate that the message wasn't delivered for some reason. ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that are returned when an invalid use of "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.)
 */
string Executor::run_WHOIS(vector<string> args, int fd) {
	// Any number of parameters is valid.
	Client* caller = getClientByFD(fd);
	if (caller == NULL) {
		return "USER NOT FOUND\n";
	}

	if (args.empty() || args[0].empty()) { // No args queries the caller.
		string userinfo = caller->getUsername() + " " + caller->getHostname() + " * :" + caller->getRealname();
		return build_WHOIS_reply(RPL_WHOISUSER, caller->getNickname(), caller->getNickname(), userinfo);
	}

	string message;

	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		Client* requestee = getClientByNickname(*it);
		if (requestee == NULL) {
			message += build_reply(ERR_NOSUCHNICK, caller->getNickname(), *it, "No such nickname");
		} else {
			string userinfo = requestee->getUsername() + " " + requestee->getHostname() + " * :" + requestee->getRealname(); //#TODO fix servername? (servername == *)
			message += build_WHOIS_reply(RPL_WHOISUSER, caller->getNickname(), *it, userinfo);
		}
	}

	return message;
}

/*
 * Incoming message: JOIN [<channel> | <password>]+
 * Parameters: The channel names, separated by a comma, to join and the channel key.
 * Description: Joins the given channels.
 *
 * Possible replies:
 * Handled:
 * RPL_TOPIC			(When sending a TOPIC message to determine the channel topic, one of two replies is sent. If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.)
 * ERR_NEEDMOREPARAMS	()
 * ERR_BADCHANNELKEY	()
 * ERR_CHANNELISFULL	()
 * ERR_NOSUCHCHANNEL	()
 * 
 * Not yet handled:
 * ERR_INVITEONLYCHAN	("<channel> :Cannot join channel (+i)"  Any command requiring operator privileges to operate must return this error to indicate the attempt was unsuccessful.)
 * ERR_TOOMANYCHANNELS	(Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.)
 * 
 * Not handled:
 * ERR_BADCHANMASK		(???)
 * ERR_BANNEDFROMCHAN	()
 */
string Executor::run_JOIN(vector<string> args, int fd) {
	Client* client = getClientByFD(fd);
	if (client == NULL) { // shit... this shouldn't happen. It means there is no registered client for the user sending the command.
		return "USER NOT FOUND\n";
	}
	string target = args[0];
	if (target[0] == '0') { // /JOIN 0 = leave all joined channels.
		//leave all channels;
		return "";
	}
	string pwds = "";
	if (args.size() == 2) {
		pwds = args[1];
	}

	map<string, string> joininfo;
	vector<string> channelnames = split_args(target);
	vector<string> channelpasswords = split_args(pwds);
	for (size_t i = 0; i < channelnames.size(); i++) {
		string channelpassword = "";
		if (i < channelpasswords.size()) {
			channelpassword = channelpasswords[i];
		}
		joininfo[channelnames[i]] = channelpassword;
	}

	string message;
	for (const auto& pair : joininfo) {
		const string& channelname = pair.first;
		const string& channelpassword = pair.second;
		Channel* ch = getChannelByName(channelname);

		if (ch == NULL) { // Create new channel and have user join as the first member.
			addChannel(channelname, channelpassword, client);
			message += build_reply(RPL_TOPIC, client->getNickname(), channelname, "Welcome to channel " + channelname); //if succesfull, reply with channel topic.
		} else {
			string password = ch->getPassword();
			const vector<Client>& clients = ch->getClients();

			if (clients.size() >= ch->getUserLimit()) {
				message += build_reply(ERR_CHANNELISFULL, client->getNickname(), ch->getName(), "Cannot join channel (+l)");
				continue;
			}

			if (find(clients.begin(), clients.end(), *client) != clients.end()) {
				message += build_reply(ERR_USERONCHANNEL, client->getNickname(), ch->getName(), "is already on channel");
				continue;
			}

			if (password.empty()) {
				ch->addClient(*client);
				message += build_reply(RPL_TOPIC, client->getNickname(), channelname, ch->getTopic()); //if succesfull, reply with channel topic.
				continue;
			}

			if (password.compare(channelpassword) != 0) {
				message += build_reply(ERR_BADCHANNELKEY, client->getNickname(), channelname, "Cannot join channel (+k)");
				continue;
			}
			ch->addClient(*client);
			message += build_reply(RPL_TOPIC, client->getNickname(), channelname, ch->getTopic()); //if succesfull, reply with channel topic.
		}
	}
	return message;
}

// Operator client wants to kick a user from a channel. #TODO check that the caller has the correct mode.
/*
 * Incoming message: KICK [<channel>] <nick>[,<nick>] [<reason>]
 * Parameters: The channel and the nicknames, separated by a comma, to kick from the channel and the reason thereof; if no channel is given, the active channel will be used.
 * Description: Removes the given nicknames from the specified channel; this command is typically used to remove troublemakers, flooders or people otherwise making a nuisance of themselves.
 * 
 * Possible replies:
 * Handled:
 *
 * Not yet handled:
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOSUCHCHANNEL	(Used to indicate the given channel name is invalid.)
 * ERR_CHANOPRIVSNEEDED (Any command requiring 'chanop' privileges (such as MODE messages) must return this error if the client making the attempt is not a chanop on the specified channel.)
 * ERR_NOTONCHANNEL		(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 * 
 * Not handled:
 * ERR_BADCHANMASK		(???)
 */
string Executor::run_KICK(vector<string> args, int fd) {
	Client* caller = getClientByFD(fd);
	if (caller == NULL) { // This shouldn't happen yo.
		return "USER NOT FOUND";
	}

	string message;
	string channelname = args[0];
	Channel* ch = getChannelByName(channelname);
	if (ch == NULL) {
		return build_reply(ERR_NOSUCHCHANNEL, caller->getNickname(), channelname, "No such channel");
	}

	vector<string>::iterator reason_start = std::find_if(args.begin(), args.end(), find_reason);

	for (vector<string>::iterator name_it = std::next(args.begin()); name_it != reason_start; name_it++) {
		Client* client = getClientByNickname(*name_it);
		if (client == NULL) {
			message += build_reply(ERR_NOSUCHNICK, caller->getNickname(), *name_it, "No such nickname");
			continue;
		}

		else if (ch->removeClient(*client) == 1) {
			message += build_channel_reply(ERR_USERNOTINCHANNEL, caller->getNickname(), *name_it, channelname, "Cannot kick user from a channel that they have not joined");
			continue;
		}

		message += ":" + this->e.server_address + " KICK " + channelname + " " + *name_it + " " + *reason_start + "\n";
	}

	return message;
}

/*
 * Incoming message: PART [<channel>[,<channel>]] [<message>] 
 * Parameters: The channels, separated by a comma, to leave and the message to advertise.
 * Description: Leaves the given channels.
 * 
 * Possible replies:
 * Handled:
 *
 * Not yet handled:
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOSUCHCHANNEL	(Used to indicate the given channel name is invalid.)
 * ERR_NOTONCHANNEL		(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 *
 * Not handled:
 */
string Executor::run_PART(vector<string> args, int fd) {
	Client* caller = getClientByFD(fd);
	if (caller == NULL) { // THis shouldn't happen yo.
		return "USER NOT FOUND\n";
	}

	vector<string> channelnames = split_args(args[0]);
	vector<string>::iterator reason_it = std::find_if(args.begin(), args.end(), find_reason);
	string message = "";

	for (vector<string>::iterator it = channelnames.begin(); it != channelnames.end(); it++) {
		Channel* ch = getChannelByName(*it);
		if (ch == NULL) {
			message += build_reply(ERR_NOSUCHCHANNEL, caller->getNickname(), *it, "No such channel");
			continue;
		}

		if (ch->removeClient(*caller) == 1) {
			message += build_reply(ERR_USERNOTINCHANNEL, caller->getNickname(), *it, "You haven't joined that channel");
			continue;
		}

		message += ":" + this->e.server_address + " PART " + *it + "\n";
		if (reason_it == args.end()) {
			cout << "skipping reason (#TODO change to default message)" << endl;
			continue;
		}

		// If there's a reason:
		// Send reason to all other users in channel, to inform them why the user left. //#TODO PROBABLY SHOULD USE SEND DIRECTLY TO FD INSTEAD OF SENDING A REPLY TO THE CLIENT.
		for (const Client& user : ch->getClients()) {
			if (user.getFD() == caller->getFD())
				continue;
			string 	reasonmessage  = ":" + caller->getNickname() + "!" + caller->getUsername() + "@" + caller->getHostname() + " ";
					reasonmessage += "PRIVMSG " + user.getNickname() + " " + *reason_it + "\n";
			send_to_client(fd, reasonmessage);
		}
	}

	return message;
}

/*
 * Incoming message: INVITE <nick> [<channel>]
 * Parameters: The nickname to invite and the channel to invite him or her to; if no channel is given, the active channel will be used.
 * Description: Invites the specified nick to a channel.
 *
 * Possible replies:
 * Handled:
 *
 * Not yet handled:
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOSUCHCHANNEL	(Used to indicate the given channel name is invalid.)
 * ERR_NOTONCHANNEL		(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 *
 * Not handled:
 */
string Executor::run_INVITE(vector<string> args, int fd) {
	Client* client = getClientByFD(fd);
	(void) client;
	(void)args;

	return "";
}

/*
 * Incoming message: TOPIC [-delete] [<channel>] [<topic>]
 * Parameters: The channel and topic; if no channel is given, the active channel will be used. If no argument is given, the current topic will be displayed.
 * Description: Displays or modifies the topic of a channel.
 *
 * Possible replies:
 *
 * Handled:
 *
 * Not yet handled:
 * RPL_TOPIC			()
 * RPL_NOTOPIC			()
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOTONCHANNEL		()
 * ERR_CHANOPRIVSNEEDED	()
 *
 * Not handled:
 */
string Executor::run_TOPIC(vector<string> args, int fd) {
	Client* client = getClientByFD(fd);
	(void) client;
	(void)args;

	return "";
}

#include <unistd.h>

// Client wants to disconnect from server
/*
 * Incoming message: QUIT [<message>]
 * Parameters: The message to advertise.
 * Description: Terminates the application and advertises the given message on all the networks you are connected to.
 * Extra info: A client session is ended with a quit message. The server must close
 *		the connection to a client which sends a QUIT message. If a "Quit
 *		Message" is given, this will be sent instead of the default message,
 *		the nickname. 
 *		If, for some other reason, a client connection is closed without the
 *		client issuing a QUIT command (e.g. client dies and EOF occurs
 *		on socket), the server is required to fill in the quit message with
 *		some sort of message reflecting the nature of the event which
 *		caused it to happen.
 *
 * Possible replies: NONE
 */
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
	stringstream response;
	response << std::setw(3) << std::setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return ":" + this->e.server_address + " " + response.str() + " " + callername + " " + target + " :" + message + "\n";
}

string Executor::build_notice_reply(string callername, string target, string message) {
	return ":" + this->e.server_address + " NOTICE " + callername + " " + target + " :" + message + "\n";
}

string Executor::build_channel_reply(int response_code, string callername, string target, string channel, string message) {
	if (response_code == NOTICE) {
		return build_notice_reply(target, callername, message);
	}
	stringstream response;
	response << std::setw(3) << std::setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return ":" + this->e.server_address + " " + response.str() + " " + callername + " " + target + " " + channel + " :" + message + "\n";
}

string Executor::build_WHOIS_reply(int response_code, string callername, string target, string userinfo) {
	stringstream response;
	response << std::setw(3) << std::setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return ":" + this->e.server_address + " " + response.str() + " " + callername + " " + target + " " + userinfo + "\n";
}
