/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Executor.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 13:56:29 by ageels        #+#    #+#                 */
/*   Updated: 2024/02/01 16:13:49 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Executor.hpp"


Channel& Executor::getChannelByName(const string name) {
	return this->e.getChannelByName(name);
}
const deque<Client *>& Executor::getClients() const {
	return this->e.getClients();
}
deque<Channel>& Executor::getChannels() {
	return this->e.getChannels();
}
const string &Executor::getHostname() const {
	return this->e.getHostname();
}

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
bool Executor::parseUserArguments(const vector<string>& args, string& username, string& hostname, string& servername, string& realname) {
	username = args[0];
	hostname = args[1];
	servername = args[2];
	realname = args[3].substr(1);

	for (size_t i = 4; i < args.size(); i++) {
		realname += " " + args[i];
	}
	return !username.empty() && is_name(username) && !hostname.empty() && !servername.empty() && (!realname.empty() || args[3] != ":");
}

void Executor::addChannel(const string& name, const string& password, Client& caller) {
	this->getChannels().emplace_back(name, password, this->e.getClients());
	Channel& chan = this->getChannels().back();
	chan.addOperator(caller);
	chan.addMember(caller);
}

bool Executor::name_exists(const string& name) {
	for (const Client* c : this->getClients()) {
		const string& clientName = c->getNickname();

		if (compare_lowercase(name, clientName)) {
			return true;
		}
	}
	return false;
}

string Executor::build_reply(int response_code, const string& caller, const string& message) {
	return build_reply(response_code, caller, "", "", message);
}

string Executor::build_reply(int response_code, const string& caller, const string& target, const string& message) {
	return build_reply(response_code, caller, target, "", message);
}

string Executor::build_reply(int response_code, const string& caller, const string& target, const string& channel, const string& message) {
	stringstream response;
	
	response << ":" << this->getHostname() << " ";

	// Response code
	if (response_code == NOTICE) {
		response << "NOTICE";
	} else {
		response << setw(3) << setfill('0') << response_code; // Ensures response_code is shown as a 3-digit number by adding leading zeroes if needed.
	}

	// Caller
	response << " " << caller;

	// Optional target
	if (!target.empty())
		response << " " << target;

	// Optional channel
	if (!channel.empty())
		response << " " << channel;

	// The message
	if (message.front() == ':')
		return response.str() + " " + message + "\r\n";
	return response.str() + " :" + message + "\r\n";
}


/*
 * Incoming message: CAP LS
 */
string Executor::run_CAP([[maybe_unused]]const vector<string>& args, [[maybe_unused]]Client& caller) {
	return "";
}

/*
 * Incoming message: PASS <password>
 */
string Executor::run_PASS(const vector<string>& args, Client& caller) {
	if (!caller.getNickname().empty() && !caller.getUsername().empty()) {
		return build_reply(ERR_ALREADYREGISTERED, caller.getNickname(), "You may not reregister");
	}
	string newpassword = args[0];
	caller.setPassword(newpassword);
	if (e.getPass().compare(newpassword) != 0) {
		caller.expell();
		return build_reply(ERR_PASSWDMISMATCH, caller.getNickname(), "Password incorrect");
	}
	return "";
}

/*
 * Incoming message: NICK <nickname>
 * Parameters: Your new nickname.
 */
string Executor::run_NICK(const vector<string>& args, Client& caller) {
	const string& new_nickname = std::string(args[0].begin(), args[0].begin() + std::min(NAME_MAX_SIZE, args[0].size()));

	if (new_nickname.empty() || !is_name(new_nickname)) {
		return build_reply(ERR_ERRONEOUSNICKNAME, "NICK", new_nickname, "Erroneous nickname");
	}

	if (name_exists(new_nickname)) {
		if (caller.getNickname().empty())
			return build_reply(ERR_NICKNAMEINUSE, new_nickname, new_nickname, "Nickname is already in use");
		else
			return build_reply(ERR_NICKNAMEINUSE, caller.getNickname(), new_nickname, "Nickname is already in use");
	}

	string old_nickname = caller.getNickname();
	string old_fullname = caller.getFullName();
	bool first_time = old_nickname.empty();
	caller.setNickname(new_nickname);

	if (first_time && !caller.getUsername().empty()) {
		return build_reply(RPL_WELCOME, new_nickname, "Welcome to Astrid's & Thibauld's IRC server, " + caller.getUsername() + "!");
	} else if (first_time) {
		return "";
	} else {
		return (":" + old_fullname + " NICK :" + new_nickname + "\r\n");
	}
}

/*
 * Incoming message: USER <username> <hostname> <servername> :<realname>
 */
string Executor::run_USER(const vector<string>& args, Client& caller) {
	string username, hostname, servername, realname;
	if (!parseUserArguments(args, username, hostname, servername, realname)) {
		if (caller.getNickname().empty()) {
			return build_reply(ERR_ERRONEOUSNICKNAME, "USER", "USER", "Invalid user arguments");
		}
		return build_reply(ERR_ERRONEOUSNICKNAME, caller.getNickname(), "USER", "Invalid user arguments");
	}
	if (caller.getUsername().empty()) {
		caller.setUsername(username);
		caller.setHostname(hostname);
		caller.setServername(servername);
		caller.setRealname(realname);
	} else
		return build_reply(ERR_ALREADYREGISTERED, caller.getNickname(), username, "You may not reregister");

	if (!caller.getNickname().empty())
		return build_reply(RPL_WELCOME, caller.getNickname(), "Welcome to Astrid's & Thibauld's IRC server, " + username + "!");
	return "";
}

/* Incoming message: PING {no args}
 * Possible replies: PONG <server>
 *
 * Responses handled:
 * PONG <server>
 */
string Executor::run_PING([[maybe_unused]]const vector<string>& args, [[maybe_unused]]Client& caller) {
	return "PONG " + this->getHostname() + "\r\n";
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
 */
string Executor::run_PRIVMSG(const vector<string>& args, Client& caller) {
	string target = args[0];

	stringstream data;
	for (vector<string>::const_iterator it = next(args.begin()); it != args.end(); it++) {
		data << *it;
		if (next(it) != args.end())
			data << " ";
	}
	data << "\r\n";
	string message = ":" + caller.getFullName() + " PRIVMSG " + target + " " + data.str();

	if (is_channel(target)) {
		Channel& target_channel = this->getChannelByName(target);
		if (target_channel == Channel::nullchan)
			return build_reply(ERR_NOSUCHNICK, caller.getNickname(), target, "No such channel");
		if (!target_channel.hasMember(caller))
			return build_reply(ERR_NOTONCHANNEL, caller.getNickname(), target, "You're not a member of that channel.");

		target_channel.sendMessageToOtherMembers(caller, message);
		return "";
	}

	Client& recipient = e.getClientByNick(target);
	if (recipient == Client::nullclient) {
		return build_reply(ERR_NOSUCHNICK, caller.getNickname(), target, "No such recipient");
	}

	recipient.addSendData(message);
	return "";
}

string Executor::run_NOTICE(const vector<string>& args, Client& caller) {
	string target = args[0];

	stringstream data;
	for (vector<string>::const_iterator it = next(args.begin()); it != args.end(); it++) {
		data << *it;
		if (next(it) != args.end())
			data << " ";
	}
	data << "\r\n";
	string message = ":" + caller.getFullName() + " NOTICE " + target + " " + data.str();

	if (is_channel(target)) {
		Channel& target_channel = this->getChannelByName(target);
		if (target_channel == Channel::nullchan)
			return "";
		if (!target_channel.hasMember(caller))
			return "";

		target_channel.sendMessageToOtherMembers(caller, message);
		return "";
	}

	Client& recipient = e.getClientByNick(target);
	if (recipient == Client::nullclient) {
		return "";
	}

	recipient.addSendData(message);
	return "";
}

/*
 * Incoming message: JOIN [<channel> | <password>]+
 * Parameters: The channel names, separated by a comma, to join and the channel key.
 * Description: Joins the given channels.
 */
string Executor::run_JOIN(const vector<string>& args, Client& caller) {
	string target = args[0];
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

		if (ch == Channel::nullchan) {
			if (is_channel(channelname)) {
				addChannel(channelname, channelpassword, caller);
				message += build_reply(RPL_TOPIC, caller.getNickname(), channelname, "Welcome to channel " + channelname);
			} else {
				message += build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), channelname, "Invalid channel name");
			}
		} else {
			string password = ch.getPassword();
			const vector<int>& clients = ch.getMembers();
			size_t userLimit = ch.getUserLimit();

			if (userLimit != 0 && clients.size() >= userLimit) {
				message += build_reply(ERR_CHANNELISFULL, caller.getNickname(), ch.getName(), "Cannot join channel (+l)");
				continue;
			}

			if (find(clients.begin(), clients.end(), caller.getFD()) != clients.end()) {
				message += build_reply(ERR_USERONCHANNEL, caller.getNickname(), caller.getNickname(), "is already on channel");
				continue;
			}

			if (ch.isInviteOnly() && !ch.hasInvited(caller)) {
				message += build_reply(ERR_INVITEONLYCHAN, caller.getNickname(), channelname, "Cannot join channel (+i)");
				continue ;
			}

			if (!password.empty() && password.compare(channelpassword) != 0 && !ch.hasInvited(caller)) {
				message += build_reply(ERR_BADCHANNELKEY, caller.getNickname(), channelname, "Cannot join channel (+k)");
				continue;
			}

			ch.broadcastToChannel(":" + caller.getFullName() + " JOIN :" + channelname + "\r\n");
			ch.addMember(caller);

			message += build_reply(RPL_TOPIC, caller.getNickname(), channelname, ch.getTopic()); //if succesfull, reply with channel topic.
		}
	}
	return message;
}


/*
 * Incoming message: KICK [<channel>] <nick>[,<nick>] [<reason>]
 * Parameters: The channel and the nicknames, separated by a comma, to kick from the channel and the reason thereof; if no channel is given, the active channel will be used.
 * Description: Removes the given nicknames from the specified channel; this command is typically used to remove troublemakers, flooders or people otherwise making a nuisance of themselves.
 */
string Executor::run_KICK(const vector<string>& args, Client& caller) {
	string message;
	string channelname = args[0];
	Channel& ch = this->getChannelByName(channelname);
	if (ch == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), channelname, "No such channel");
	}
	if (!ch.hasMember(caller)) {
		return build_reply(ERR_NOTONCHANNEL, channelname, "You're not in that channel");
	}

	vector<string>::const_iterator reason_start = find_if(args.begin(), args.end(), find_reason);

	for (vector<string>::const_iterator target_it = next(args.begin()); target_it != reason_start; target_it++) {
		if (!ch.hasOperator(caller)) {
			message += build_reply(ERR_CHANOPRIVSNEEDED, channelname, "You're not the channel operator");
			continue;
		}
		Client& victim = e.getClientByNick(*target_it);
		if (victim == Client::nullclient) {
			message += build_reply(ERR_NOSUCHNICK, caller.getNickname(), *target_it, "No such nickname");
			continue;
		}
		else if (!ch.hasMember(victim)) {
			message += build_reply(ERR_USERNOTINCHANNEL, channelname, "They aren't on that channel");
			continue;
		}
		string reason;
		if (reason_start == args.end() || (*reason_start).compare(":") == 0) {
			reason = ":" + *target_it;
		} else {
			reason = *reason_start;
		}		
		ch.broadcastToChannel( ":" + caller.getFullName() + " KICK " + channelname + " " + *target_it + " " + reason + "\r\n");
		ch.removeMember(victim);
		victim.addSendData(":" + this->getHostname() + " You were kicked from " + channelname + " by " + caller.getNickname() + " " + reason + "\r\n");
	}
	if (ch.empty()) {
		this->getChannels().erase(this->e.getItToChannelByName(ch.getName()));
	}
	return message;
}

/*
 * Incoming message: PART [<channel>[,<channel>]] [<message>] 
 * Parameters: The channels, separated by a comma, to leave and the message to advertise.
 * Description: Leaves the given channels.
 */
string Executor::run_PART(const vector<string>& args, Client& caller) {
	vector<string> channelnames = split_args(args[0]);
	vector<string>::const_iterator reason_it = find_if(args.begin(), args.end(), find_reason);
	string reason = ":user " + caller.getNickname() + " parted";
	string message = "";

	if (reason_it != args.end()) {
		reason = *reason_it;
	}

	for (vector<string>::iterator it = channelnames.begin(); it != channelnames.end(); it++) {
		Channel& ch = this->getChannelByName(*it);
		if (ch == Channel::nullchan) {
			message += build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), *it, "No such channel");
			continue;
		}

		if (ch.removeMember(caller) == false) {
			message += build_reply(ERR_NOTONCHANNEL, caller.getNickname(), *it, "You're not on that channel");
			continue;
		}

		// INFO : irssi doesn't handle the PART message in the right way so I made it KICK instead so the behavior is correct.
		message += ":" + caller.getFullName() + " KICK " + *it + " " + caller.getNickname() + " " + reason + "\r\n";

		if (ch.empty()) {
			this->getChannels().erase(this->e.getItToChannelByName(*it));
		} else {
			ch.broadcastToChannel(":" + caller.getFullName() + " PART " + *it + " " + reason + "\r\n");
		}
	}

	return message;
}

/*
 * Incoming message: INVITE <nick> [<channel>]
 * Parameters: The nickname to invite and the channel to invite him or her to; if no channel is given, the active channel will be used.
 * Description: Invites the specified nick to a channel.
 */
string Executor::run_INVITE(const vector<string>& args, Client& caller) {
	string target_client = args[0];
	string target_channel = "";
	if (args.size() > 0)
		target_channel = args[1];

	Channel& channel = this->getChannelByName(target_channel);
	if (channel == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), target_channel, "No such channel");
	}
	if (!channel.hasMember(caller))
		return build_reply(ERR_NOTONCHANNEL, target_channel, "You're not on that channel");
	if (channel.isInviteOnly() && !channel.hasOperator(caller))
		return build_reply(ERR_CHANOPRIVSNEEDED, target_channel, "You're not a channel operator");

	Client& target = e.getClientByNick(target_client);
	if (target == Client::nullclient)
		return build_reply(ERR_NOSUCHNICK, target_channel, target_client, "No such nick");
	if (channel.hasMember(target))
		return build_reply(ERR_USERONCHANNEL, target_channel, target_client, "is already on channel");

	channel.addInvited(target);
	target.addSendData(":" + caller.getFullName() + " INVITE " + target.getNickname() + " " + target_channel + "\r\n");

	return	build_reply(RPL_INVITING, caller.getNickname(), target_client, target_channel);
}

/*
 * Incoming message: TOPIC [-delete] [<channel>] [<topic>]
 * Parameters: The channel and topic; if no channel is given, the active channel will be used. If no argument is given, the current topic will be displayed.
 * Description: Displays or modifies the topic of a channel.
*/
string Executor::run_TOPIC(const vector<string>& args, Client& caller) {
	string	target_channel;
	string	newtopic = "";

	target_channel = args[0];
	
	if (args.size() > 1)
		newtopic = args[1];

	Channel& channel = this->getChannelByName(target_channel);
	if (channel == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, caller.getNickname(), target_channel, "No such channel");
	}
	if (!channel.hasMember(caller)) {
		return build_reply(ERR_NOTONCHANNEL, caller.getNickname(), target_channel, "You're not on that channel");
	}

	const string& oldtopic = channel.getTopic();

	if (newtopic.empty()) {
		if (oldtopic.empty()) {
			return build_reply(RPL_NOTOPIC, caller.getNickname(), target_channel, "No topic is set");
		}
		return build_reply(RPL_TOPIC, caller.getNickname(), target_channel, oldtopic);
	}

	if (channel.hasTopicRestricted() && !channel.hasOperator(caller)) {
		return build_reply(ERR_CHANOPRIVSNEEDED, target_channel, "You're not a channel operator");
	}
	channel.setTopic(newtopic);
	channel.broadcastToChannel(":" + caller.getFullName() + " TOPIC " + channel.getName() + " " + newtopic + "\r\n");
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
 */

string Executor::run_QUIT(const vector<string>& args, Client& caller) {
	deque<Channel>& channels = this->getChannels();
	
	string	fullName = caller.getFullName();
	string	reason = ":" + caller.getNickname();

	caller.expell();

	if (!args.empty())
		reason = args[0];

	for (auto it = channels.begin(); it != channels.end(); it++) {
		if ((*it).hasOperator(caller))
			(*it).removeOperator(caller);
		if ((*it).hasMember(caller)) {
			(*it).removeMember(caller);
			(*it).broadcastToChannel(":" + fullName + " QUIT " + reason + "\r\n");
		}
	}

    auto newEnd = std::remove_if(channels.begin(), channels.end(), channel_is_empty);
    channels.erase(newEnd, channels.end());
	return "";
}


Executor::Executor(const Executor &src) : e(src.e) {
	*this = src;
	Msg("Executor - copy constructor", "CLASS");
}
Executor &Executor::operator=(const Executor &src) { // ??
	(void)src;
	return(*this);
	Msg("Executor - assignment operator", "CLASS");
}

//public

Executor::Executor(Env& e) : e(e) {
	this->funcMap["CAP"] 		= &Executor::run_CAP;
	this->funcMap["PASS"] 		= &Executor::run_PASS;
	this->funcMap["NICK"] 		= &Executor::run_NICK;
	this->funcMap["USER"] 		= &Executor::run_USER;
	this->funcMap["MODE"] 		= &Executor::run_MODE;
	this->funcMap["PING"] 		= &Executor::run_PING;
	this->funcMap["PRIVMSG"] 	= &Executor::run_PRIVMSG;
	this->funcMap["NOTICE"] 	= &Executor::run_NOTICE;
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
	this->argCount["NOTICE"]	= {2, -1};
	this->argCount["JOIN"] 		= {1, 2};
	this->argCount["KICK"] 		= {1, -1};
	this->argCount["PART"] 		= {1, -1};
	this->argCount["INVITE"] 	= {1, 2};
	this->argCount["TOPIC"] 	= {1, -1};
	this->argCount["QUIT"] 		= {0, -1};
	Msg("Executor - constructor(env &e)", "CLASS");
}
Executor::~Executor() {
	Msg("Executor - default destructor", "CLASS");
}

int Executor::run(const Command& cmd, Client& caller) {
	string message;
	string command = cmd.getCommand();
	mbrFuncPtr ptr;

	if (caller.isExpelled() == true)
		return false;
	
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
		caller.addSendData(build_reply(ERR_PASSWDMISMATCH, caller.getNickname(), "Password needed"));
		return false;
	}

	// Check user is registered for commands other than PASS, CAP, NICK, USER.
	static vector<string> regcmds = {"PASS", "CAP", "NICK", "USER"};
	if (find(regcmds.begin(), regcmds.end(), command) == regcmds.end()) {
		if (!caller.isRegistered())
		{
			caller.addSendData(build_reply(ERR_NOTREGISTERED, caller.getNickname(), "You have not registered (correctly)"));
			return false;
		}
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

	caller.addSendData(message);
	if (caller.isExpelled())
		return false;
	return true;
}
