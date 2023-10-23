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

class Executor; // Forward declaration

typedef string (Executor::*mbrFuncPtr)(void*, vector<string>);

class Executor {
	private:
		// vector<string> valid_commands;
		std::map<string, mbrFuncPtr> funcMap;
	public:
		Executor();
		~Executor();

		void send_to_client(int fd, string message);
		void run(void* env, Command& cmd, int fd);

		string run_CAP(void* env, vector<string> args);
		string run_NICK(void* env, vector<string> args);
		string run_USER(void* env, vector<string> args);
		string run_MODE(void* env, vector<string> args);
		string run_PING(void* env, vector<string> args);
		string run_PRIVMSG(void* env, vector<string> args);
		string run_WHOIS(void* env, vector<string> args);
		string run_JOIN(void* env, vector<string> args);
		string run_KICK(void* env, vector<string> args);
		string run_QUIT(void* env, vector<string> args);

};

#endif /* end of include guard: EXECUTOR_HPP */
