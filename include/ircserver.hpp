#pragma once

#include <string>
	using std::string;
#include <vector>
	using std::vector;

#include "Msg.hpp"
#include "Server.hpp"
#include "Executor.hpp"
#include "Env.hpp"
#include "SignalHandler.hpp"

class Executor; // Forward declaration

int				parse(int argc, char **argv);

bool			is_channel(const string& name);
bool			is_nickname_character(const char c);
bool			verify_name(const string& arg);
bool			verify_realname(const string& arg);
vector<string>	split_args(const string& args);
bool			compare_lowercase(const string& a, const string& b);