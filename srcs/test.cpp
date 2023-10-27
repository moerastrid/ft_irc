#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

void test(env& env, string& incoming, int fd) {
	static Executor ex;

	Command cmd(incoming, fd);

	string reply = ex.run(env, cmd, fd);

	cout << "Processing Incoming message: [" + incoming + "]" << endl;
	cout << "Reply received: " << endl;
	cout << "[" << reply << "]" << endl;
	cout << "===============================" << endl << endl;
}

int main() {
	env env;
	env.server_address = "localhost";

	// Input:
	int fd_user1 = 4;
	string message = "NICK neus\n";
	string message2 = "USER thibauld thibauld_PC localhost :Thibauld WW Nuyten\n";

	int fd_user2 = 5;
	string message3 = "NICK astrid\n";
	string message4 = "USER astrid astrid_PC localhost :astrid geels\n";

	string message5 = "PING :localhost\n";
	string message6 = "WHOIS neus astrid doofus\n";

	string message7 = "JOIN #test,#test2,#test3,#test4 123,234,345\n";
	string message8 = "JOIN #test3, qwerty\n";
	string message9 = "JOIN #test4\n";
	string message10 = "JOIN \n";

	string message11 = "KICK \n";
	string message12 = "KICK #test neus :go away\n";
	string message13 = "KICK #test neus :go away\n";

	string message14 = "JOIN #test 123\n";
	string message15 = "PART #test :you guys suck\n";


	// NICK + USER for two users
	test(env, message, fd_user1);
	test(env, message2, fd_user1);
	test(env, message3, fd_user2);
	test(env, message4, fd_user2);

	// PING
	test(env, message5, fd_user1);

	// WHOIS
	test(env, message6, fd_user1);

	// JOIN test
	test(env, message7, fd_user1);
	test(env, message8, fd_user1);
	test(env, message9, fd_user1);
	test(env, message10, fd_user1);

	// KICK test
	test(env, message11, fd_user1);
	test(env, message12, fd_user1);
	test(env, message13, fd_user1);

	for (const auto& el : env.clients) {
		cout << el << endl;
	}
	
	// PART test
	test(env, message14, fd_user1);
	test(env, message14, fd_user2);
	test(env, message15, fd_user1);

	for (const auto& el : env.clients) {
		cout << el << endl;
	}
}
