#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

// class Test {
// public:
// 	vector<string&> strings;
// }

int main() {

	// string s1 = "hello doofus";
	// vector<string&> vec;
	// vec.push_back(s1);
	//
	// Test t;
	// t.strings = vec;
	//
	// cout << t.strings[0] << endl;

	// Input:
	int fd = 0;
	string message = "NICK thibauld 123 test \n";
	env env;

	env.server_address = "localhost";

	// setup
	Executor ex;

	// parse
	Command cmd(message, fd);

	//output from parser:
	cout << cmd.getCommand() << endl;
	vector<string>args = cmd.getArgs();
	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		cout << *it << endl;
	}

	// run
	string received = ex.run(env, cmd, fd);

	cout << "Message_received: [" << received << "]" << endl;
}