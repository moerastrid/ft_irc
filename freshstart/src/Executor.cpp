#include "Executor.hpp"

// some utilities #TODO move

//https://modern.ircdocs.horse/#invite-message

bool is_channel_char(char c) {
	return isalnum(c) || string("!#$%'()+,-./").find(c) != string::npos;
}

bool is_channel(const string& name) {
	for (const auto& c : name) {
		if (!is_channel_char(c))
			return false;
	}
	return !name.empty() && name.front() == '#';
}

bool is_nickname_character(const char c) {
	string valid = "_-[]\\^{}";
	if (isalnum(c) || valid.find(c) != string::npos)
		return true;
	return false;
}

bool verify_name(const string& arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_nickname_character(*it))
			return false;
	}
	return true;
}

bool verify_realname(const string& arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_nickname_character(*it) && !isspace(*it))
			return false;
	}
	return true;
}

// Takes a comma-separated string of arguments, gives back a vector of said arguments.
vector<string> split_args(const string& args) {
	istringstream nameStream(args);
	vector<string> res;

	string buffer;
	while (getline(nameStream, buffer, ',')) {
		res.push_back(buffer);
	}

	return res;
}

//case insensitive string comparison
bool compare_lowercase(const string& a, const string& b) {
	string ac = a;
	string bc = b;

	transform(ac.begin(), ac.end(), ac.begin(), [](unsigned char c) {
		return tolower(c);
	});

	transform(bc.begin(), bc.end(), bc.begin(), [](unsigned char c) {
		return tolower(c);
	});

	return ac == bc;
}

bool Executor::name_exists(const string& name) {
	for (const Client& c : this->getClients()) {
		const string& clientName = c.getNickname();

		if (compare_lowercase(name, clientName)) {
			return true;
		}
	}
	return false;
}

// Executor class

Executor::Executor(Env& e) : e(e) {
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
	this->argCount["MODE"] 		= {1, -1};
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

int Executor::validateArguments(const string& command, int numArgs) {
	if (this->argCount.find(command) != this->argCount.end()) {
		pair<int, int> argCounts = this->argCount[command];
		int minArgs = argCounts.first;
		int maxArgs = argCounts.second;

		if (numArgs < minArgs) {
			return -1;
		}
		if (maxArgs >= 0 && numArgs > maxArgs) {
			return 1;
		}
	}

	return 0;
}

int Executor::run(const Command& cmd, Client& caller) {
	string message;
	string command = cmd.getCommand();
	mbrFuncPtr ptr;
	
	// Handle unknown commands
	try {
		ptr = this->funcMap.at(command);
	}
	catch (std::out_of_range& e) {
		message = build_reply(ERR_UNKNOWNCOMMAND, caller.getNickname(), command, "Unknown command from client");
		caller.addSendData(message);
		return true;
	}

	// Check password on commands other than PASS and CAP
	static vector<string> passcmds = {"PASS", "CAP"};
	if (find(passcmds.begin(), passcmds.end(), command) == passcmds.end() && caller.getPassword().empty()) {
		return false;
	}

	// Check user is registered for commands other than PASS, CAP, NICK, USER.
	static vector<string> regcmds = {"PASS", "CAP", "NICK", "USER"};
	if (find(regcmds.begin(), regcmds.end(), command) == regcmds.end()) {
		if (!caller.isRegistered())
			return false;
	}

	int numArgs = cmd.getArgs().size();

	int res = validateArguments(command, numArgs);
	if (res == -1) {
		message = build_reply(ERR_NEEDMOREPARAMS, caller.getNickname(), command, "Not enough parameters");
	} else if (res == 1) {
		message = build_reply(ERR_TOOMANYPARAMS, caller.getNickname(), command, "Too many parameters");
	} else {
		message = (this->*ptr)(cmd.getArgs(), caller);
	}

	if (message.find("Nickname collision KILL") != string::npos)
		return false;	
	if (message.find("QUITTING") != string::npos)
		return false;
	caller.addSendData(message);

	return true;
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
string Executor::run_CAP([[maybe_unused]]const vector<string>& args, [[maybe_unused]]Client& caller) {
	return "";
	// return "CAP NAK :-\n";
	return "CAP * LS :-\n"; // Correct reply for the correct case, but if the client starts doing weird things, capability negotiation does not follow protocol. 
	//We probably just want to ignore CAP completely. :(
}

/*
 * Incoming message: PASS <password> (not a user command, sent by the server if a password is supplied)
 * Possible replies: ???
 *
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED
 * ERR_PASSWDMISMATCH
 *
 * Responses not (yet) handled:
 */
string Executor::run_PASS(const vector<string>& args, Client& caller) {
	if (!caller.getNickname().empty() || !caller.getUsername().empty()) {
		return build_reply(ERR_ALREADYREGISTERED, caller.getNickname(), caller.getNickname(), "You may not reregister");
	}

	string newpassword = args[0];

	if (e.getPass().compare(newpassword) != 0) {
		// #TODO close connection? and send ERROR
		return build_reply(ERR_PASSWDMISMATCH, "PASS", "PASS", "Password incorrect"); //# TODO FIX MESSAGE FORMAT
	}

	caller.setPassword(newpassword);

	return ""; // build_reply(NOTICE, "PASS", "PASS", "Remember to set your username and nickname with USER and PASS.");
}

/*
 * Incoming message: NICK <nickname>
 * Parameters: Your new nickname.
 * Description: Changes your nickname on the active server.
 * Possible replies:
 *
 * Responses handled:
 * ERR_NICKCOLLISION
 * ERR_ERRONEUSNICKNAME
 *
 * Responses not yet handled:
 *
 * Responses not handled:
 * ERR_NICKNAMEINUSE (override by ERR_NICKCOLLISION)
 * ERR_NONICKNAMEGIVEN (override by ERR_NEEDMOREPARAMS)
 *
 */
string Executor::run_NICK(const vector<string>& args, Client& caller) {
	const string& nickname = args[0];

	// if (caller.getPassword().empty()) // Checked in run().
	// 	return build_notice_reply(caller.getNickname(), caller.getNickname(), "Enter the server password first with PASS to start connection registration");

	if (name_exists(nickname)) { // #TODO Figure out nickcollision vs nicknameinuse
		cout << "caller name :" << caller.getNickname() << ":\n";
		if (caller.getNickname().empty())
			return build_reply(ERR_NICKCOLLISION, "NICK", nickname, "Nickname collision KILL from "+ caller.getUsername() + "@" + caller.getHostname());
		else
			return build_reply(ERR_NICKNAMEINUSE, "NICK", nickname, "Nickname is already in use");
	}
	if (nickname.empty() || !verify_name(nickname)) {
		return build_reply(ERR_ERRONEOUSNICKNAME, "NICK", nickname, "Erroneous nickname");
	}

	bool first_time = caller.getNickname().empty();
	caller.setNickname(nickname);

	if (first_time && !caller.getUsername().empty()) { // First time connection part 2: electric boogaloo. Accepting connection and sending welcome message.
		return build_reply(RPL_WELCOME, nickname, nickname, "Welcome to Astrid's & Thibauld's IRC server, " + caller.getUsername() + "!");
	} else if (first_time) {
		return "";
	} else {
		return build_reply(RPL_WELCOME, nickname, nickname, "Nickname changed to " + nickname);
	}
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
 * Responses added:
 * ERR_ERRONEOUSNICKNAME
 * RPL_WELCOME
 *
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED
 *
 * Responses not yet handled:
 *
 * Responses not handled:
 *
 */
string Executor::run_USER(const vector<string>& args, Client& caller) {
	string username, hostname, servername, realname;
	if (!parseUserArguments(args, username, hostname, servername, realname)) {
		if (caller.getNickname().empty()) {
			return build_reply(ERR_ERRONEOUSNICKNAME, "USER", "USER", "Invalid user arguments");
		}
		return build_reply(ERR_ERRONEOUSNICKNAME, caller.getNickname(), "USER", "Invalid user arguments");
	}

	if (caller.getUsername().empty()) {// first time connec part 2
		caller.setUsername(username);
		caller.setHostname(hostname);
		caller.setServername(servername);
		caller.setRealname(realname);
	} else
		return build_reply(ERR_ALREADYREGISTERED, caller.getNickname(), username, "You may not reregister");

	if (!caller.getNickname().empty())
		return build_reply(RPL_WELCOME, caller.getNickname(), username, "Welcome to Astrid's & Thibauld's IRC server, " + username + "!");
	return "";
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
string Executor::run_PING([[maybe_unused]]const vector<string>& args, [[maybe_unused]]Client& caller) {
	return "PONG " + this->e.getHostname() + "\n";
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
string Executor::run_PRIVMSG(const vector<string>& args, Client& caller) {
	string target = args[0];

	stringstream data;
	for (vector<string>::const_iterator it = next(args.begin()); it != args.end(); it++) {
		data << *it;
		if (next(it) != args.end())
			data << " ";
	}
	data << "\n";


	if (is_channel(target)) {
		Channel& target_channel = this->getChannelByName(target);
		if (target_channel == Channel::nullchan)
			return build_reply(ERR_NOSUCHNICK, caller.getNickname(), target, "No such channel");
		std::vector<Client *>& channel_members = target_channel.getMembers();
		for (Client* member: channel_members) {
			if (member != &caller)
				member->sendPrivMsg(data.str());
		}
		return "";
	}

	Client& recipient = this->getClientByNick(target);
	if (recipient == Client::nullclient) {
		return build_reply(ERR_NOSUCHNICK, caller.getNickname(), target, "No such recipient");
	}


	recipient.sendPrivMsg(data.str());
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
string Executor::run_WHOIS(const vector<string>& args, Client& caller) {
	if (args.empty() || args[0].empty()) { // No args queries the caller.
		string userinfo = caller.getUsername() + " " + caller.getHostname() + " * :" + caller.getRealname();
		return build_WHOIS_reply(RPL_WHOISUSER, caller.getNickname(), caller.getNickname(), userinfo);
	}

	string message;

	for (vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
		Client& requestee = this->getClientByNick(*it);
		if (requestee == NULL) {
			message += build_reply(ERR_NOSUCHNICK, caller.getNickname(), *it, "No such nickname");
		} else {
			string userinfo = requestee.getUsername() + " " + requestee.getHostname() + " * :" + requestee.getRealname(); //#TODO fix servername? (servername == *)
			message += build_WHOIS_reply(RPL_WHOISUSER, caller.getNickname(), *it, userinfo);
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
string Executor::run_JOIN(const vector<string>& args, Client& caller) {
	string target = args[0];
	if (target[0] == '0') { // /JOIN 0 = leave all joined channels.
		//#TODO leave all channels;
		std::deque<Channel>& channels = this->e.getChannels();
		for (Channel &chan : channels) {
			if (chan.hasMember(caller)) {
				chan.removeMember(caller);
				if (chan.empty())
					channels.erase(this->e.getItToChannelByName(chan.getName()));
			}
		}
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
		Channel& ch = this->getChannelByName(channelname);

		if (ch == Channel::nullchan) { // Create new channel and have user join as the first member.
			addChannel(channelname, channelpassword, caller);
			message += build_reply(RPL_TOPIC, caller.getNickname(), channelname, "Welcome to channel " + channelname); //if succesfull, reply with channel topic.
		} else {
			string password = ch.getPassword();
			const vector<Client *>& clients = ch.getMembers();
			size_t userLimit = ch.getUserLimit();
			if (userLimit != 0 && clients.size() >= userLimit) {
				message += build_reply(ERR_CHANNELISFULL, caller.getNickname(), ch.getName(), "Cannot join channel (+l)");
				continue;
			}

			if (find(clients.begin(), clients.end(), &caller) != clients.end()) {
				message += build_reply(ERR_USERONCHANNEL, caller.getNickname(), ch.getName(), "is already on channel");
				continue;
			}

			if (password.empty()) {
				ch.addMember(caller);
				message += build_reply(RPL_TOPIC, caller.getNickname(), channelname, ch.getTopic()); //if succesfull, reply with channel topic.
				continue;
			}

			if (password.compare(channelpassword) != 0) {
				message += build_reply(ERR_BADCHANNELKEY, caller.getNickname(), channelname, "Cannot join channel (+k)");
				continue;
			}
			ch.addMember(caller);
			message += build_reply(RPL_TOPIC, caller.getNickname(), channelname, ch.getTopic()); //if succesfull, reply with channel topic.
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
string Executor::run_KICK(const vector<string>& args, Client& caller) {
	string message;
	string channelname = args[0];
	Channel& ch = this->getChannelByName(channelname);
	if (ch == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), channelname, "No such channel");
	}

	vector<string>::const_iterator reason_start = find_if(args.begin(), args.end(), find_reason);

	for (vector<string>::const_iterator name_it = next(args.begin()); name_it != reason_start; name_it++) {
		Client& client = this->getClientByNick(*name_it);
		if (client == Client::nullclient) {
			message += build_reply(ERR_NOSUCHNICK, caller.getNickname(), *name_it, "No such nickname");
			continue;
		}

		else if (ch.removeMember(client) == 1) {
			message += build_channel_reply(ERR_USERNOTINCHANNEL, caller.getNickname(), *name_it, channelname, "Cannot kick user from a channel that they have not joined");
			continue;
		}

		message += "KICK " + channelname + " " + *name_it + " " + *reason_start + "\n";
	}
	if (ch.empty()) {
		this->e.getChannels().erase(this->e.getItToChannelByName(ch.getName()));
	}

	return message;
}

/*
 * Incoming message: PART [<channel>[,<channel>]] [<message>] 
 * Parameters: The channels, separated by a comma, to leave and the message to advertise.
 * Description: Leaves the given channels.
 *
 * Possible replies:
 * Handled:addChannel
 *
 * Not yet handled:
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOSUCHCHANNEL	(Used to indicate the given channel name is invalid.)
 * ERR_NOTONCHANNEL		(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 *
 * Not handled:
 */
string Executor::run_PART(const vector<string>& args, Client& caller) {
	vector<string> channelnames = split_args(args[0]);
	vector<string>::const_iterator reason_it = find_if(args.begin(), args.end(), find_reason);
	string message = "";

	for (vector<string>::iterator it = channelnames.begin(); it != channelnames.end(); it++) {
		Channel& ch = this->getChannelByName(*it);
		if (ch == Channel::nullchan) {
			message += build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), *it, "No such channel");
			continue;
		}

		if (ch.removeMember(caller) == 1) {
			message += build_reply(ERR_USERNOTINCHANNEL, caller.getNickname(), *it, "You haven't joined that channel");
			continue;
		}

		if (ch.empty()) {
			this->e.getChannels().erase(this->e.getItToChannelByName(ch.getName()));
			continue;
		}

		message += "PART " + *it + "\n";
		if (reason_it == args.end()) {
			cout << "skipping reason (#TODO change to default message)" << endl;
			continue;
		}
		
		// If there's a reason:
		// Send reason to all other users in channel, to inform them why the user left. //#TODO PROBABLY SHOULD USE SEND DIRECTLY TO FD INSTEAD OF SENDING A REPLY TO THE CLIENT.
		for (Client* user : ch.getMembers()) {
			if (user->getFD() == caller.getFD())
				continue;
			string 	reasonmessage  = ":" + caller.getNickname() + "!" + caller.getUsername() + "@" + caller.getHostname() + " ";
					reasonmessage += "PRIVMSG " + user->getNickname() + " " + *reason_it + "\n";
			// send_to_client(fd, reasonmessage); #TODO fix
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
 * ERR_NEEDMOREPARAMS	()
 * ERR_NOSUCHCHANNEL	(Used to indicate the given channel name is invalid.)
 * ERR_NOTONCHANNEL		(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 * ERR_CHANOPRIVSNEEDED	()
 * ERR_NOSUCHNICK		()
 * ERR_USERONCHANNEL	()
 *
 * RPL_INVITING			()
 *
 * Not yet handled:
 *
 * Not handled:
 * RPL_AWAY				()
 */
string Executor::run_INVITE(const vector<string>& args, Client& caller) {
	string target_client = args[0];
	string target_channel = args[1];
	Channel& channel = this->getChannelByName(target_channel);
	if (channel == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), target_channel, "No such channel");
	}

	if (!channel.hasMember(caller)) {
		return build_reply(ERR_NOTONCHANNEL, caller.getNickname(), target_channel, "Not on channel");
	}

	if (channel.isInviteOnly() && !channel.hasOperator(caller)) {
		return build_reply(ERR_CHANOPRIVSNEEDED, caller.getNickname(), target_channel, "You're not a channel operator");
	}

	Client& target = this->getClientByNick(target_client);
	if (target == NULL) {
		return build_reply(ERR_NOSUCHNICK, caller.getNickname(), target_client, "No such client");
	}

	if (channel.hasMember(target)) {
		return build_channel_reply(ERR_USERONCHANNEL, caller.getNickname(), target_client, target_channel, "is already on channel");
	}

	// #TODO send private message to invitee in in :nick!user@host form.
	/*
	 * Message Examples:

		:dan-!d@localhost INVITE Wiz #test ; dan- has invited Wiz to the channel #test
	 *
	 */

	return build_channel_reply(RPL_INVITING, caller.getNickname(), target_client, target_channel, ""); // #TODO if message is empty, dont send the message part.
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
string Executor::run_TOPIC(const vector<string>& args, Client& caller) {
	string deleteflag = args[0];
	string target_channel;
	string newtopic = "";

	if (deleteflag.compare("-delete")) {
		target_channel = args[1];
	} else {
		target_channel = args[0];
		newtopic = args[1];
	}

	Channel& channel = this->getChannelByName(target_channel);

	const string& oldtopic = channel.getTopic();

	if (!channel.hasMember(caller)) {
		return build_reply(ERR_NOTONCHANNEL, caller.getNickname(), target_channel, "You're not on that channel");
	}

	if (newtopic.empty()) {
		if (oldtopic.empty()) {
			return build_reply(RPL_NOTOPIC, caller.getNickname(), target_channel, "No topic is set");
		}
		return build_reply(RPL_TOPIC, caller.getNickname(), target_channel, oldtopic);
	}

	if (channel.hasTopicRestricted() && !channel.hasOperator(caller)) {
		return build_reply(ERR_CHANOPRIVSNEEDED, caller.getNickname(), target_channel, "You're not a channel operator");
	}

	// #TODO send message to all clients on the channel informing them of the topic change.

	return "";
}

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
string Executor::run_QUIT([[maybe_unused]]const vector<string>& args,[[maybe_unused]] Client& caller) {
	deque<Channel>& channels = this->getChannels();

	for (auto& chan : channels) {
		if (chan.hasOperator(caller))
			chan.removeOperator(caller);
		if (chan.hasMember(caller))
			chan.removeMember(caller);
		
		if (chan.empty()) {
			this->e.getChannels().erase(this->e.getItToChannelByName(chan.getName()));
		}
	}

	return "QUITTING";
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

void Executor::addChannel(const string& name, const string& password, Client& caller) {
	// Channel ch(name, password);
	// ch.addMember(caller);
	// this->getChannels().push_back(ch);

	this->getChannels().emplace_back(name, password);
	Channel& chan = this->getChannels().back();
	chan.addOperator(caller);
	chan.addMember(caller);
}

string Executor::format_reason(vector<string>::iterator& reason_start, vector<string>& args) {
	string message = "";
	for (vector<string>::iterator it = reason_start; it != args.end(); it++) {
		message += *it;
		if (next(it) != args.end()) {
			message += " ";
		}
	}
	return message;
}

string Executor::build_reply(int response_code, string callername, string target, string message) {
	if (response_code == NOTICE) {
		return build_notice_reply(target, callername, message);
	}
	stringstream response;
	response << setw(3) << setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return response.str() + " " + callername + " " + target + " :" + message + "\n";
}

string Executor::build_notice_reply(string callername, string target, string message) {
	return "NOTICE " + callername + " " + target + " :" + message + "\n";
}

string Executor::build_channel_reply(int response_code, string callername, string target, string channel, string message) {
	if (response_code == NOTICE) {
		return build_notice_reply(target, callername, message);
	}
	stringstream response;
	response << setw(3) << setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return response.str() + " " + callername + " " + target + " " + channel + " :" + message + "\n";
}

string Executor::build_WHOIS_reply(int response_code, string callername, string target, string userinfo) {
	stringstream response;
	response << setw(3) << setfill('0') << response_code; // Ensures response_code is shows as a 3-digit number by adding leading zeroes if needed.

	return response.str() + " " + callername + " " + target + " " + userinfo + "\n";
}

std::ostream& operator<<(std::ostream& os, const Env& e) {
	os << "env(" << e.getHostname() << ", " << e.getIP() << ", "<< e.getPort() << ")";
	return os;
}

Client& Executor::getClientByNick(const string nick) {
	return this->e.getClientByNick(nick);
}

Channel& Executor::getChannelByName(const string name) {
	return this->e.getChannelByName(name);
}

deque<Client>& Executor::getClients() {
	return this->e.getClients();
}

deque<Channel>& Executor::getChannels() {
	return this->e.getChannels();
}