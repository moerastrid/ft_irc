#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
using std::string;
#include <vector>
using std::vector;

class Command {
private:
	string command;
	vector<string> args;
public:
	Command();
	Command(string cmd, vector<string> args);
	~Command();
	const Command& operator=(const Command& other);

	void addArg(string arg);
	void setCmd(string cmd);
	string getCommand();
	string getArg(size_t idx);
	vector<string> getArgs();
};

#endif /* end of include guard: COMMAND_HPP */