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

typedef string (Executor::*mbrFuncPtr)(vector<string>, int fd);

class Executor {
	private:
		std::map<string, mbrFuncPtr> funcMap;
		env& e;
	public:
//		Executor();
		Executor(env& e);
		~Executor();

		void send_to_client(int fd, string message);
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
		string run_QUIT(vector<string> args, int fd);
		string run_PART(vector<string> args, int fd);

		Client* getClientByFD(int fd);
		Channel* getChannelByName(string name);
		Client* getClientByNickname(string nickname);
		vector<Client>::iterator getItToClientByNickname(string nickname);

		void addClient(string username, string nickname, string hostname, string servername, string realname, int fd);
		bool parseUserArguments(const vector<string>& args, string& username,
				string& hostname, string& servername, string& realname);
		void addChannel(string name, string password, Client* client);

		string format_reason(vector<string>::iterator& reason_it, vector<string>& args);
		string error_message(int error_code, string a, string b);

};

#endif /* end of include guard: EXECUTOR_HPP */
