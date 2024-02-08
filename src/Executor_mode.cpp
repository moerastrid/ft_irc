/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Executor_mode.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 14:43:53 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:30:23 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Executor.hpp"

static bool is_mode(char c) {
	string modes = "itkol";
	if (modes.find(c) != string::npos) {
		return true;
	}
	return false;
}

static bool hasArg(char mode, bool add) {
	string validPos = "kol";
	string validNeg = "o";
	if (add) {
		if (validPos.find(mode) != string::npos)
			return true;
	}
	else {
		if (validNeg.find(mode) != string::npos)
			return true;
	}
	return false;
}

/*
 * Tuple parameters:
 * bool: add or remove option for mode.
 * char: the mode
 * string: mode argument
 */
static vector<tuple<bool, signed char, string>> parse_modestring(string modestring, vector<string> modeargs) {
	bool add = true;
	int argcounter = 0;
	vector<tuple<bool, signed char, string>> modes;
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
			if (hasArg(el, add)) {
				try {
					arg = modeargs.at(argcounter++);
				} catch (std::out_of_range& e) {
					modes.push_back(std::tuple<bool, signed char, string>(add, -1, "")); // MISSING PARAMETER
					continue;
				}
			}
			modes.push_back(std::tuple<bool, signed char, string>(add, el, arg));
		} else {
			modes.push_back(std::tuple<bool, signed char, string>(add, el, "")); // UNKNOWN MODE
		}
	}
	return modes;
}

static bool check_privileges(const Client& caller, const Channel& target, const string& modestring) {
	bool res = true;
	if (modestring.find('i') != string::npos) {
		res = res && target.hasOperator(caller);
	}
	if (modestring.find('t') != string::npos) {
		if (target.hasTopicRestricted())
			res = res && target.hasOperator(caller);
	}
	if (modestring.find('k') != string::npos)
		res = res && target.hasOperator(caller);
	if (modestring.find('o') != string::npos)
		res = res && target.hasOperator(caller);
	if (modestring.find('l') != string::npos)
		res = res && target.hasOperator(caller);
	return res;
}

static pair<string,string> get_current_modes(const Channel& ch) {
	string modes = "+";
	string args = "";
	string password = ch.getPassword();
	size_t userLimit = ch.getUserLimit();
	std::cerr << "Userlimit: " << userLimit << endl;
	if (!password.empty()) {
		modes += 'k';
		args += password;
	}
	if (ch.hasTopicRestricted()) {
		modes += 't';
	}
	if (ch.isInviteOnly()) {
		modes += 'i';
	}
	if (userLimit != 0) {
		modes += 'l';
		args += userLimit;
	}
	return std::make_pair(modes, args);
}

static string return_modestring(bool add, char mode) {
	string res = "-";
	if (add) {
		res = "+";
	}
	res += mode;
	return res;
}

// Type D
void Executor::handle_i_mode(const bool add, Channel& target) {
	if (add)
		target.makeInviteOnly();
	else
		target.takeInviteOnly();
}

// Type D
void Executor::handle_t_mode(const bool add, Channel& target) {
	if (add)
		target.makeTopicOperatorOnly();
	else
		target.takeTopicOperatorOnly();
}

// Type B: If set and no parameter, ignore command.
void Executor::handle_k_mode(const bool add, const string& arg, Channel& target) {
	if (add) {
		if (!arg.size())
			return ;
		target.setPassword(arg);
	}
	else {
		target.setPassword("");
	}
}

// Type A: must always have parameter, otherwise ignore command.
void Executor::handle_o_mode(const bool add, const string& arg, Channel& target) {
	if (!arg.size())
		throw std::invalid_argument("");
	Client& c = this->e.getClientByNick(arg);
	if (c == Client::nullclient)
		throw std::domain_error("");
	if (!target.hasMember(c))
		throw std::runtime_error("");
	if (add)
		target.addOperator(c);
	else
		target.removeOperator(c);
	return ;
}

// Type C: If set and no parameter, ignore command.
void Executor::handle_l_mode(const bool add, const string& arg, Channel& target) {
	int num;
	if (add) {
		try {
			num = std::stoi(arg);
		} catch (const std::exception& e) {
			throw std::runtime_error("");
		}
		std::cerr << "num: " << num << endl;
		if (num > 1)
			target.setUserLimit(num);
		else
			throw std::runtime_error("");
	}
	else
		target.takeInviteOnly();
}

string Executor::handle_modes(const Client& caller, const vector<tuple<bool, signed char, string>>& mode_cmds, Channel& target) {
	string message = "";
	for (const auto& mode_cmd: mode_cmds) {
		bool add = std::get<0>(mode_cmd);
		signed char mode = std::get<1>(mode_cmd);
		string modearg = std::get<2>(mode_cmd);

		switch (mode)
		{
			case 'i':
				handle_i_mode(add, target);
				message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
				break;
			case 't':
				handle_t_mode(add, target);
				message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
				break;
			case 'k':
				if (modearg.empty() && add) {
					modearg = target.getPassword();
					message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
					continue;
				}
				handle_k_mode(add, modearg, target);
				message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
				break;
			case 'o':
				try {
					handle_o_mode(add, modearg, target);
					message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
				} catch (const std::invalid_argument& e) {
						message += build_reply(ERR_NEEDMOREPARAMS, caller.getNickname(), modearg, "Invalid or missing parameter");
				} catch (const std::domain_error& e) {
						message += build_reply(ERR_NOSUCHNICK, caller.getNickname(), modearg, "No such nick");
				} catch (const std::runtime_error& e) {
						message += build_reply(ERR_USERNOTINCHANNEL, caller.getNickname(), target.getName(), "They aren't on that channel");
				}
				
				break;
			case 'l':
				try {
					handle_l_mode(add, modearg, target);
					message += build_mode_reply(caller.getNickname(), target.getName(), return_modestring(add, mode), modearg);
				} catch (const std::runtime_error& e) {
					message += build_reply(ERR_NEEDMOREPARAMS, target.getName(), "Invalid mode parameter: [" + modearg + "]");
				}
				break;
			case 0:
				message += build_reply(ERR_UMODEUNKNOWNFLAG, target.getName(), "Unknown MODE flag: [" + to_string(mode) + "]");
				break;
			case -1: //ignored
				break;
			default:
				string test = "";
				test += mode;
				message += build_reply(ERR_UMODEUNKNOWNFLAG, target.getName(), "Unknown MODE flag: [" + test + "]");
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
 */
string Executor::run_MODE(const vector<string>& args, Client& caller) {

	string target = args[0];
	Client& target_client = this->e.getClientByNick(target);
	Channel& channel = this->getChannelByName(target);
	bool target_is_channel = is_channel(target);

	if (!target_is_channel) {
		if (target_client == Client::nullclient)
			return build_reply(ERR_NOSUCHNICK, target, "No such nick/channel");
		else if (caller.getNickname().compare(target) != 0)
			return this->build_reply(ERR_USERSDONTMATCH, target, "Cant change mode for other users");
		else
			return "";
	}
	if (channel == Channel::nullchan) {
		return build_reply(ERR_NOSUCHCHANNEL, target, "No such channel");
	}
	if (args.size() == 1) { // No modestring
		pair<string, string> replymodes = channel.getModes();
		return build_mode_reply(caller.getNickname(), target, replymodes.first, replymodes.second);
	}

	string modestring = args[1];
	vector<string> modeargs;
	size_t i = 2;
	while (i < args.size()) {
		modeargs.push_back(args[i++]);
	}

	if (check_privileges(caller, channel, modestring) == false) {
		return build_reply(ERR_CHANOPRIVSNEEDED, target, "You're not an operator of " + target);
	}

	if (!modestring.size()) {
		pair<string,string> modedata = get_current_modes(channel);
		string modestring = modedata.first;
		string modeargs = modedata.second;
		return build_mode_reply(caller.getNickname(), target, modestring, modeargs);
	}

	vector<tuple<bool, signed char, string>> mode_cmds = parse_modestring(modestring, modeargs);
	return handle_modes(caller, mode_cmds, channel);
}

string Executor::build_mode_reply(string callername, string target_channel, string modestring, string modeargs) {
	string message = ":" + this->getHostname() + " " + to_string(RPL_CHANNELMODEIS) + " " + callername + " " + target_channel + " " + modestring;
	if (!modeargs.empty()) {
		message += " " + modeargs;
	}
	message += "\r\n";
	return message;
}
