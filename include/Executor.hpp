#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

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
#define ERR_NOTONCHANNEL 442 		//(Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.)
#define ERR_USERONCHANNEL 443
#define ERR_NEEDMOREPARAMS 461
#define ERR_TOOMANYPARAMS 461
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_KEYSET 467				// "<channel> :Channel key already set"
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_BADCHANNELKEY 475
#define ERR_CHANOPRIVSNEEDED 482 

#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502 // Cant change mode for other users

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
	using std::endl;

#include <iomanip>
	using std::setw;
	using std::setfill;

#include <sys/socket.h>
#include <sys/types.h>
#include <stdexcept>

#include "Command.hpp"
#include "Env.hpp"

class Executor; // Forward declaration

typedef string (Executor::*mbrFuncPtr)(const vector<string>&, Client&);

class Executor {
	private:
		map<string, mbrFuncPtr> funcMap;
		map<string, pair<int, int>> argCount;
		Env& e;
	public:
//		Executor();
		Executor(Env& e);
		~Executor();

		int					validateArguments(const string& command, int numArgs);
		int					run(const Command& cmd, Client& caller);

		string				run_CAP(const vector<string>& args, Client& caller);
		string				run_PASS(const vector<string>& args, Client& caller);
		string				run_NICK(const vector<string>& args, Client& caller);
		string				run_USER(const vector<string>& args, Client& caller);
		string				run_MODE(const vector<string>& args, Client& caller);
		string				run_PING(const vector<string>& args, Client& caller);
		string				run_PRIVMSG(const vector<string>& args, Client& caller);
		string				run_WHOIS(const vector<string>& args, Client& caller);
		string				run_JOIN(const vector<string>& args, Client& caller);
		string				run_KICK(const vector<string>& args, Client& caller);
		string				run_PART(const vector<string>& args, Client& caller);
		string				run_INVITE(const vector<string>& args, Client& caller);
		string				run_TOPIC(const vector<string>& args, Client& caller);
		string				run_QUIT(const vector<string>& args, Client& caller);

		string				handle_modes(const Client& caller, const vector<tuple<bool, signed char, string>>& mode_cmds, Channel& target);
		void				handle_i_mode(const bool add, Channel& target);
		void				handle_t_mode(const bool add, Channel& target);
		void				handle_k_mode(const bool add, const string& arg, Channel& target);
		void				handle_o_mode(const bool add, const string& arg, Channel& target);
		void				handle_l_mode(const bool add, const string& arg, Channel& target);

		// void				addClient(string username, string nickname, string hostname, string servername, string realname, int fd);
		void				addChannel(const string& name, const string& password, Client& caller);

		bool				name_exists(const string& name);
		bool				parseUserArguments(const vector<string>& args, string& username,
								string& hostname, string& servername, string& realname);

		string				format_reason(vector<string>::iterator& reason_it, vector<string>& args);
		string				new_build_reply(string& prefix, int response_code, string& caller, string& target, string& channel, string& message);
		string				build_reply(int response_code, string callername, string target, string message);
		string				build_notice_reply(string callername, string target, string message);
		string				build_channel_reply(int response_code, string callername, string target, string channel, string message);
		string				build_WHOIS_reply(int response_code, string callername, string target, string userinfo);
		string				build_mode_reply(string callername, string target_channel, string modestring, string modeargs);

		Client&				getClientByNick(const string nick);
		Channel&			getChannelByName(const string name);
		deque<Client>&		getClients();
		deque<Channel>&		getChannels();
};

bool			is_channel(const string& name);
bool			is_nickname_character(const char c);
bool			verify_name(const string& arg);
bool			verify_realname(const string& arg);
vector<string>	split_args(const string& args);

#endif /* end of include guard: EXECUTOR_HPP */
