#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

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

typedef string (Executor::*mbrFuncPtr)(env&, vector<string>, int fd);

class Executor {
	private:
		std::map<string, mbrFuncPtr> funcMap;
	public:
		Executor();
		~Executor();

		void send_to_client(int fd, string message);
		string run(env& env, Command& cmd, int fd);

		string run_CAP(env& env, vector<string> args, int fd);
		string run_NICK(env& env, vector<string> args, int fd);
		string run_USER(env& env, vector<string> args, int fd);
		string run_MODE(env& env, vector<string> args, int fd);
		string run_PING(env& env, vector<string> args, int fd);
		string run_PRIVMSG(env& env, vector<string> args, int fd);
		string run_WHOIS(env& env, vector<string> args, int fd);
		string run_JOIN(env& env, vector<string> args, int fd);
		string run_KICK(env& env, vector<string> args, int fd);
		string run_QUIT(env& env, vector<string> args, int fd);
		string run_PART(env& env, vector<string> args, int fd);

		Client* getClientByFD(env& env, int fd);
		Channel* getChannelByName(env& env, string name);
		Client* getClientByNickname(env& env, string nickname);
		vector<Client>::iterator getItToClientByNickname(env& env, string nickname);

		void addClient(env& env, string username, string nickname, string hostname, string servername, string realname, int fd);
		bool parseUserArguments(const vector<string>& args, string& username,
				string& hostname, string& servername, string& realname);
		void addChannel(env& env, string name, string password, Client* client);

		string format_reason(vector<string>::iterator& reason_it, vector<string>& args);
		string error_message(int error_code, string a, string b);

};

#endif /* end of include guard: EXECUTOR_HPP */
