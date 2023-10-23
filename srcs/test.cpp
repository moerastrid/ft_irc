#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main() {

	// Input:
	int fd = 0;
	string message2 = "NICK thibauld\n";
	string message = "USER thibauld thibauld THIBAULD :Thibauld\n";
	// string message2 = "USER thibauld thibauld THIBAULD :Thibauld ww Nuyten\n";
	env env;

	env.server_address = "localhost";

	// setup
	Executor ex;

	// parse
	Command cmd(message, fd);
	Command cmd2(message2, fd);

	//output from parser:
	cout << cmd.getCommand() << endl;
	vector<string>args = cmd.getArgs();
	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		cout << *it << endl;
	}

	cout << cmd2.getCommand() << endl;
	vector<string>args2 = cmd2.getArgs();
	for (vector<string>::iterator it = args2.begin(); it != args2.end(); it++) {
		cout << *it << endl;
	}

	// run
	string received = ex.run(env, cmd, fd);
	string received2 = ex.run(env, cmd2, fd);
	// ex.send_to_client(fd, received);

	cout << "Message_received: [" << received << "]" << endl;
	cout << "Message_received: [" << received2 << "]" << endl;
}