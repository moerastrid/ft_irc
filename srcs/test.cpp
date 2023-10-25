#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;


int main() {

	// Input:
	int fd_user1 = 4;
	string message = "NICK neus\n";
	string message2 = "USER thibauld thibauld_PC localhost :Thibauld WW Nuyten\n";

	int fd_user2 = 5;
	string message3 = "NICK astrid\n";
	string message4 = "USER astrid astrid_PC localhost :astrid geels\n";

	string message5 = "PING :localhost\n";
	string message6 = "WHOIS neus astrid doofus\n";
	env env;

	env.server_address = "localhost";

	// setup
	Executor ex;

	// parse
	Command cmd(message, fd_user1);
	Command cmd2(message2, fd_user1);
	Command cmd3(message3, fd_user2);
	Command cmd4(message4, fd_user2);
	Command cmd5(message5, fd_user1);
	Command cmd6(message6, fd_user1);

	// run
	string received = ex.run(env, cmd, fd_user1);
	string received2 = ex.run(env, cmd2, fd_user1);
	string received3 = ex.run(env, cmd3, fd_user2);
	string received4 = ex.run(env, cmd4, fd_user2);
	string received5 = ex.run(env, cmd5, fd_user1);
	string received6 = ex.run(env, cmd6, fd_user1);

	cout << "Message_received: [" << received << "]" << endl;
	cout << "Message_received: [" << received2 << "]" << endl;
	cout << "Message_received: [" << received3 << "]" << endl;
	cout << "Message_received: [" << received4 << "]" << endl;
	cout << "Message_received: [" << received5 << "]" << endl;
	cout << "Message_received: [" << received6 << "]" << endl;
}
