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
using std::tuple;

#include <string>
using std::string;
using std::getline;

#include <map>
using std::map;

#include <cctype>
using std::isspace;

#include <iterator>
using std::next;

#include <sstream>
using std::istringstream;

#include <algorithm>
using std::find;
using std::find_if;

#include <iomanip>
using std::setw;
using std::setfill;

#include <sys/socket.h>
#include <sys/types.h>
#include <stdexcept>

#include "Command.hpp"
#include "env.hpp"

class Executor; // Forward declaration

typedef string (Executor::*mbrFuncPtr)(vector<string>, int fd);

class Executor {
	private:
		map<string, mbrFuncPtr> funcMap;
		map<string, pair<int, int>> argCount;
		env& e;
	public:
//		Executor();
		Executor(env& e);
		~Executor();

		int validateArguments(const string& command, int numArgs);
		string run(Command& cmd, int fd);

		string run_CAP(vector<string> args, int fd);
		string run_PASS(vector<string> args, int fd);
		string run_NICK(vector<string> args, int fd);
		string run_USER(vector<string> args, int fd);
		string run_MODE(vector<string> args, int fd);
		string run_PING(vector<string> args, int fd);
		string run_PRIVMSG(vector<string> args, int fd);
		string run_WHOIS(vector<string> args, int fd);
		string run_JOIN(vector<string> args, int fd);
		string run_KICK(vector<string> args, int fd);
		string run_PART(vector<string> args, int fd);
		string run_INVITE(vector<string> args, int fd);
		string run_TOPIC(vector<string> args, int fd);
		string run_QUIT(vector<string> args, int fd);

		string handle_modes(Client* caller, vector<tuple<bool, char, string>> mode_cmds, Channel* target);
		void handle_i_mode(bool add, Channel* target);
		void handle_t_mode(bool add, Channel* target);
		void handle_k_mode(bool add, string arg, Channel* target);
		void handle_o_mode(bool add, string arg, Channel* target);
		void handle_l_mode(bool add, string arg, Channel* target);

		void addClient(string username, string nickname, string hostname, string servername, string realname, int fd);
		void addChannel(string name, string password, Client* client);

		Client* getClientByFD(int fd);
		string getServerPassword();
		Channel* getChannelByName(string name);
		Client* getClientByNickname(string nickname);
		vector<Client>::iterator getItToClientByNickname(string nickname);

		bool parseUserArguments(const vector<string>& args, string& username,
								string& hostname, string& servername, string& realname);

		string format_reason(vector<string>::iterator& reason_it, vector<string>& args);
		string build_reply(int response_code, string callername, string target, string message);
		string build_notice_reply(string callername, string target, string message);
		string build_channel_reply(int response_code, string callername, string target, string channel, string message);
		string build_WHOIS_reply(int response_code, string callername, string target, string userinfo);
		string build_mode_reply(string callername, string target_channel, string modestring, string modeargs);
		string error_message(int error_code, string a, string b);

		void send_to_client(int fd, string message);
};

bool is_channel(string name);
bool is_valid_nickname_character(char c);
bool verify_name(string arg);
bool verify_realname(string arg);
vector<string> split_args(string args);

#endif /* end of include guard: EXECUTOR_HPP */
