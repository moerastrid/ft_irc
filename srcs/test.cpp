#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;


int main() {

	// Input:
	int fd = 0;
	string message = "NICK thibauld\n";
	string message2 = "USER thibauld thibauld THIBAULD :Thibauld\n";
	string message3 = "PING\n";
	env env;

	env.server_address = "localhost";

	// setup
	Executor ex;

	// parse
	Command cmd(message, fd);
	Command cmd2(message2, fd);
	Command cmd3(message3, fd);


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
	
	cout << cmd3.getCommand() << endl;
	vector<string>args3 = cmd3.getArgs();
	for (vector<string>::iterator it = args3.begin(); it != args3.end(); it++) {
		cout << *it << endl;
	}

	// run
	string received = ex.run(env, cmd, fd);
	string received2 = ex.run(env, cmd2, fd);
	string received3 = ex.run(env, cmd3, fd);
	// ex.send_to_client(fd, received);

	cout << "Message_received: [" << received << "]" << endl;
	cout << "Message_received: [" << received2 << "]" << endl;
	cout << "Message_received: [" << received3 << "]" << endl;
}
