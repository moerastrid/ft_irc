/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Executor.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 13:56:15 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:27:12 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define NOTICE 999

#define RPL_WELCOME 001
#define RPL_UMODEIS 221
#define RPL_WHOISUSER 311
#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEOUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NICKCOLLISION 436
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442 
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_TOOMANYPARAMS 461
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_KEYSET 467
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define ERR_CHANOPRIVSNEEDED 482 

#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502

#include <utility>
	using std::pair;

#include <tuple>
	using std::tuple;

#include <string>
	using std::string;
	using std::getline;

#include <map>
	using std::map;

#include <cctype>
	using std::isspace;
	using std::tolower;

#include <iterator>
	using std::next;

#include <sstream>
	using std::istringstream;

#include <algorithm>
	using std::find;
	using std::find_if;
	using std::transform;

#include <iostream>
	using std::cout;

#include <iomanip>
	using std::setw;
	using std::setfill;

#include <sys/socket.h>
#include <sys/types.h>
#include <stdexcept>

#include "ircserver.hpp"
#include "Command.hpp"
#include "Env.hpp"


class Executor {
	private:
		typedef string (Executor::*mbrFuncPtr)(const vector<string>&, Client&);
		map<string, mbrFuncPtr> funcMap;
		map<string, pair<int, int>> argCount;
		Env& e;

		// Channel&		getChannelByName(const string name);
		// deque<Client>&	getClients();
		// deque<Channel>&	getChannels();
		// const string&	getHostname() const;

		int		validateArguments(const string& command, int numArgs);
		bool	parseUserArguments(const vector<string>& args, string& username,
					string& hostname, string& servername, string& realname);

		void	addChannel(const string& name, const string& password, Client& caller);
		bool	name_exists(const string& name);

	//	string	build_short_reply(const string& prefix, const string& command, const string& postfix);
		string	new_build_reply(int response_code, const string& caller, const string& target, const string& channel, const string& message);
		string	new_build_reply(int response_code, const string& caller, const string& target, const string& message);
		string	new_build_reply(int response_code, const string& caller, const string& message);

		string	run_CAP(const vector<string>& args, Client& caller);
		string	run_PASS(const vector<string>& args, Client& caller);
		string	run_NICK(const vector<string>& args, Client& caller);
		string	run_USER(const vector<string>& args, Client& caller);
		string	run_PING(const vector<string>& args, Client& caller);
		string	run_PRIVMSG(const vector<string>& args, Client& caller);
		string	run_NOTICE(const vector<string>& args, Client& caller);
//		string	run_WHOIS(const vector<string>& args, Client& caller);
		string	run_JOIN(const vector<string>& args, Client& caller);
		string	run_KICK(const vector<string>& args, Client& caller);
		string	run_PART(const vector<string>& args, Client& caller);
		string	run_INVITE(const vector<string>& args, Client& caller);
		string	run_TOPIC(const vector<string>& args, Client& caller);
		string	run_QUIT(const vector<string>& args, Client& caller);
	
		// MODE functions are in : executor_mode.cpp
		string	run_MODE(const vector<string>& args, Client& caller); 
		
		void	handle_i_mode(const bool add, Channel& target);
		void	handle_t_mode(const bool add, Channel& target);
		void	handle_k_mode(const bool add, const string& arg, Channel& target);
		void	handle_o_mode(const bool add, const string& arg, Channel& target);
		void	handle_l_mode(const bool add, const string& arg, Channel& target);
		string	handle_modes(const Client& caller, const vector<tuple<bool, signed char, string>>& mode_cmds, Channel& target);
		string	build_mode_reply(string callername, string target_channel, string modestring, string modeargs);
		// end of MODE functions


		//Executor();
		Executor(const Executor &src);
		Executor &operator=(const Executor &src);

	public:
		Executor(Env& e);
		~Executor();

		int					run(const Command& cmd, Client& caller);


		

		// void				addClient(string username, string nickname, string hostname, string servername, string realname, int fd);

	//	string				format_reason(vector<string>::iterator& reason_it, vector<string>& args);
		
		// string				build_reply(int response_code, string callername, string target, string message);
		// string				build_notice_reply(string callername, string target, string message);
		// string				build_channel_reply(int response_code, string callername, string target, string channel, string message);
		// string				build_WHOIS_reply(int response_code, string callername, string target, string userinfo);

};

