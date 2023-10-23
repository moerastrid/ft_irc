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

	// Move constructor
	Command(Command&& other) noexcept {
		this->command = std::move(other.command);
		this->args = std::move(other.args);
	}

	// Move assignment operator
	Command& operator=(Command&& other) noexcept {
		if (this != &other) {
			this->command = std::move(other.command);
			this->args = std::move(other.args);
		}
		return *this;
	}

	void addArg(string arg);
	void setCmd(string cmd);
	string getCommand();
	string getArg(size_t idx);
	vector<string> getArgs();

	//Could make Executor class (probably better), could also just make a run function here.
};

#endif /* end of include guard: COMMAND_HPP */