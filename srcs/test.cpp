#include "MessageParser.hpp"
#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main() {
	// Input:
	int fd = 0;
	string message = "NICK thibauld 123 test \n";
	void* env = NULL;

	// setup
	MessageParser mp;
	Executor ex;

	// parse
	Command cmd = mp.parse_client_message(env, message, fd);

	//output from parser:
	cout << cmd.getCommand() << endl;
	vector<string>args = cmd.getArgs();
	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		cout << *it << endl;
	}

	// run
	ex.run(env, cmd, fd);
}