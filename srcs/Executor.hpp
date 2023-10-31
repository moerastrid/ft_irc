#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#define NOTICE 999

#define RPL_WELCOME 001

#define RPL_WHOISUSER 311
#define RPL_TOPIC 332

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEOUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_USERONCHANNEL 443
#define ERR_NEEDMOREPARAMS 461
#define ERR_TOOMANYPARAMS 461
#define ERR_UNKNOWNMODE 472
#define ERR_BADCHANNELKEY 475

#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>

#include <string>
using std::string;

#include <map>
using std::map;

#include <stdexcept>

#include "Command.hpp"
#include "env.hpp"

class Executor; // Forward declaration

typedef string (Executor::*mbrFuncPtr)(vector<string>, int fd);

class Executor {
	private:
		std::map<string, mbrFuncPtr> funcMap;
	    std::map<std::string, std::pair<int, int>> argCount;
		env& e;
	public:
//		Executor();
		Executor(env& e);
		~Executor();

	    int validateArguments(const std::string& command, int numArgs);
		string run(Command& cmd, int fd);

		string run_CAP(vector<string> args, int fd);
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

		void addClient(string username, string nickname, string hostname, string servername, string realname, int fd);
		void addChannel(string name, string password, Client* client);

		Client* getClientByFD(int fd);
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
		string error_message(int error_code, string a, string b);

		void send_to_client(int fd, string message);
};

#endif /* end of include guard: EXECUTOR_HPP */
