#include "Executor.hpp"

static bool is_mode(char c) {
	string modes = "itkol";
	if (modes.find(c) != string::npos) {
		return true;
	}
	return false;
}

static bool hasArg(char mode) {
	string valid = "kol";
	if (valid.find(mode) != string::npos)
		return true;
	return false;
}

/*
 * Tuple parameters: 
 * bool: add or remove option for mode.
 * char: the mode
 * string: mode argument
 */

static vector<tuple<bool, char, string>> parse_modestring(string modestring, vector<string> modeargs) {
	bool add = true;
	int argcounter = 0;
	vector<tuple<bool, char, string>> modes;
	for (const auto& el : modestring) {
		if (el == '+') {
			add = true;
			continue;
		}
		if (el == '-') {
			add = false;
			continue;
		}
		if (is_mode(el)) {
			string arg = "";
			if (hasArg(el)) {
				try {
					arg = modeargs.at(argcounter++);
				} catch (std::out_of_range& e) {
					modes.push_back(std::tuple<bool, char, string>(add, -1, "")); // MISSING PARAMETER
					continue;
				}
			}
			modes.push_back(std::tuple<bool, char, string>(add, el, arg));
		} else {
			modes.push_back(std::tuple<bool, char, string>(add, 0, "")); // UNKNOWN MODE
		}
	}
	return modes;
}

static bool check_privileges(Client* caller, Channel* target, string modestring) {
	bool res = true;
	if (modestring.find('i') != string::npos) {
		res = res && caller->isOperator(*target);
	}
	if (modestring.find('t') != string::npos) {
		if (target->hasTopicRestricted())
			res = res && caller->isOperator(*target);
	}
	if (modestring.find('k') != string::npos)
		res = res && caller->isOperator(*target);
	if (modestring.find('o') != string::npos)
		res = res && (caller->isFounder(*target) || caller->isOperator(*target));
	if (modestring.find('l') != string::npos)
		res = res && caller->isOperator(*target);
	return res;
}

static pair<string,string> get_current_modes(Channel* ch) {
	string modes = "+";
	string args = "";
	string password = ch->getPassword();
	size_t userLimit = ch->getUserLimit();
	if (!password.empty()) {
		modes += 'k';
		args += password;
	}
	if (ch->hasTopicRestricted()) {
		modes += 't';
	}
	if (ch->isInviteOnly()) {
		modes += 'i';
	}
	if (userLimit != 0) {
		modes += 'l';
		args += userLimit;
	}
	return std::make_pair(modes, args);
}

// Type D
void Executor::handle_i_mode(bool add, Channel* target) {
	if (add)
		target->makeInviteOnly();
	else
		target->takeInviteOnly();
}

// Type D
void Executor::handle_t_mode(bool add, Channel* target) {
	if (add)
		target->makeTopicOperatorOnly();
	else
		target->takeTopicOperatorOnly();
}

// Type B: If set and no parameter, ignore command.
void Executor::handle_k_mode(bool add, string arg, Channel* target) {
	if (add) {
		if (!arg.size()) 
			return ;
		target->setPassword(arg);
	}
	else
		target->setPassword("");
}

// Type A: must always have parameter, otherwise ignore command.
void Executor::handle_o_mode(bool add, string arg, Channel* target) {
	if (!arg.size())
		return;
	Client* c = this->getClientByNickname(arg);	
	if (c == NULL)
		return ;
	if (add)
		target->addOperator(*c);
	else
		target->removeOperator(*c);
}

// Type C: If set and no parameter, ignore command.
void Executor::handle_l_mode(bool add, string arg, Channel* target) {
	int num;
	if (add) {
		try {
			num = std::stoi(arg);
		} catch (const std::exception& e) {
			return ;
		}
		target->setUserLimit(num);
	}
	else
		target->takeInviteOnly();
}

#include <iostream>
using std::cout;
using std::endl;

string Executor::handle_modes(Client* caller, vector<tuple<bool, char, string>> mode_cmds, Channel* target) {
	string message = "";
	for (const auto& mode_cmd: mode_cmds) {
		bool add = std::get<0>(mode_cmd);
		char mode = std::get<1>(mode_cmd);
		string modearg = std::get<2>(mode_cmd);
		string modestring;

		switch (mode)
		{
			case 'i':
				handle_i_mode(add, target);
				modestring = "+i";
				message += build_mode_reply(caller->getNickname(), target->getName(), modestring, modearg);
				break;
			case 't':
				handle_t_mode(add, target);
				modestring = "+t";
				message += build_mode_reply(caller->getNickname(), target->getName(), modestring, modearg);
				break;
			case 'k':
				modestring = "+k";
				if (modearg.empty()) {
					modearg = target->getPassword();
					message += build_mode_reply(caller->getNickname(), target->getName(), modestring, "");
					continue;
				}
				handle_k_mode(add, modearg, target);
				message += build_mode_reply(caller->getNickname(), target->getName(), modestring, modearg);
				break;
			case 'o':
				modestring = "+o";
				message += build_mode_reply(caller->getNickname(), target->getName(), modestring, modearg);
				handle_o_mode(add, modearg, target); //#TODO check modearg for existence of user.
				break;
			case 'l':
				modestring = "+l";
				message += build_mode_reply(caller->getNickname(), target->getName(), modestring, modearg);
				handle_l_mode(add, modearg, target);
				break;
			case 0:
				message += build_reply(ERR_UMODEUNKNOWNFLAG, caller->getNickname(), target->getName(), "Unknown MODE flag");
				break;
			case -1: //ignored
				break;
			default:
				message += build_reply(ERR_UMODEUNKNOWNFLAG, caller->getNickname(), target->getName(), "Unknown MODE flag");
				break;
		}
	}
	return message;
}

/*
 * Incoming message		: MODE <channel> [<mode> [<mode parameters>]]
 * Mode arguments		: (set with +<mode> or -<mode>)
 * 		i				: invite only
 * 		t				: Topic change only by operator
 * 		k <key>			: Adds/removes password
 * 		o <nickname>	: Give <nickname> operator status.
 * 		l <count>		: Set max number of users for channel (0 = unlimited).
 * Parameters			: The target nickname or channel and the modes with their parameters to set or remove. If the target nickname or channel is omitted, the active nickname or channel will be used.
 * Description			: Modifies the channel modes for which you are privileged to modify. You can specify multiple modes in one command and prepend them by using the ‘+’ sign to set or ‘-’ sign to unset; modes that require a parameter will be retrieved from the argument list.
 * 
 * Possible replies		: ???
 *
 * Responses handled:
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHNICK 		(Used to indicate the nickname parameter supplied to a command is currently unused.)
 * ERR_USERSDONTMATCH 	(Error sent to any user trying to view or change the user mode for a user other than themselves.)
 *
 * Responses not yet handled:
 * ERR_NOSUCHCHANNEL 	(Requested channel is unknown)
 * ERR_UNKNOWNMODE		(Requested mode is unknown)
 * RPL_UMODEIS 			(To answer a query about a client's own mode, RPL_UMODEIS is sent back.)
 * RPL_CHANNELMODEIS 	(Response to mode status request) "<channel> <mode> <mode params>"
 * ERR_CHANOPRIVSNEEDED	(User without operator status request mode change that requires OP)
 * ERR_NOTONCHANNEL 	(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
 * ERR_KEYSET 			(Format: "<channel> Channel key already set")
 * ERR_UMODEUNKNOWNFLAG	(Returned by the server to indicate that a MODE message was sent with a nickname parameter and that the a mode flag sent was not recognized.)
 * 
 * Responses not handled:
 * RPL_BANLIST 			(We don't support a banlist)
 * RPL_ENDOFBANLIS 		(We don't support a banlist)
 */
string Executor::run_MODE(vector<string> args, int fd) {
	Client* caller = getClientByFD(fd);
	if (caller == NULL) {
		return "USER NOT FOUND";
	}

	string target = args[0];
	// Client* target_client = getClientByNickname(target);
	Channel* channel = getChannelByName(target);
	bool target_is_channel = is_channel(target);

	if (!target_is_channel) { // If target is a user. (Commented code checks properly, but we don't support modes on users, only channels)
		return build_reply(ERR_NOSUCHCHANNEL, caller->getNickname(), target, "No such channel (we don't support changing modes for users)");
		// if (target_client == NULL) // if target doesn't exist
		// 	return build_reply(ERR_NOSUCHNICK, caller->getNickname(), target, "No such nick/channel");
		// else if (caller->getNickname().compare(target) != 0) // if target doesn't match caller.
		// 	return build_reply(ERR_USERSDONTMATCH, caller->getNickname(), target, "Cant change mode for other users");
	}
	if (channel == NULL) { // If target is a channel and it doesn't exist.
		return build_reply(ERR_NOSUCHCHANNEL, caller->getNickname(), target, "No such channel");
	}
	if (args.size() == 1) { // No modestring
		pair<string, string> replymodes = channel->getModes();
		return build_mode_reply(caller->getNickname(), target, replymodes.first, replymodes.second);
	}

	string modestring = args[1];
	vector<string> modeargs;
	size_t i = 2;
	while (i < args.size()) {
		modeargs.push_back(args[i++]);
	}

	if (check_privileges(caller, channel, modestring) == false) {
		return build_reply(ERR_CHANOPRIVSNEEDED, caller->getNickname(), target, "You're not an operator of " + target);
	}

	if (!modestring.size()) {
		pair<string,string> modedata = get_current_modes(channel);
		string modestring = modedata.first;
		string modeargs = modedata.second;
		return build_mode_reply(caller->getNickname(), target, modestring, modeargs);
	}

	vector<tuple<bool, char, string>> mode_cmds = parse_modestring(modestring, modeargs);
	return handle_modes(caller, mode_cmds, channel);
}

string Executor::build_mode_reply(string callername, string target_channel, string modestring, string modeargs) {
	string message = ":" + this->e.hostname + " " + to_string(RPL_CHANNELMODEIS) + " " + callername + " " + target_channel + " " + modestring;
	if (!modeargs.empty()) {
		 message += " " + modeargs;
	}
	message += "\n";
	return message;
}

//void messageDispatch() {
//	map<string, string> m = {"RPL_CHANNELMODEIS","build-a-string magic"};
//}
