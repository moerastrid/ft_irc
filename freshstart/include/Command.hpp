#pragma once

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <sstream>
using std::stringstream;
#include <algorithm>

class Command {
private:
	string			command;
	vector<string>	args;
	int				client_fd;
public:
	Command();
	Command(const string& client_message_string, int client_fd);
	~Command();
	const Command& operator=(const Command& other);

	void addArg(string arg);
	void setCmd(string cmd);
	string getCommand();
	string getArg(size_t idx);
	vector<string> getArgs();

	void combine_reason();
};

bool find_reason(const string& str);
