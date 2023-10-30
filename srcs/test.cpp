#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

void test(env& env, string& incoming, int fd) {
	static Executor ex(env);

	cout << "Processing Incoming message: [" + incoming + "]" << endl;

	Command cmd(incoming, fd);

	cout << "Reply received: " << endl;
	string reply = ex.run(cmd, fd);
	cout << "[" << reply << "]" << endl;

	cout << "===============================" << endl << endl;
}

void setup_test(env& env) {
	int fd_user1 = 4;
	int fd_user2 = 5;

	string capreq = "CAP LS\n";

	string nick1 = "NICK neus\n";
	string user1 = "USER thibauld thibauld_PC localhost :Thibauld WW Nuyten\n";
	string mode1 = "MODE thibauld +i\n";
	string ping1 = "PING localhost\n";


	string nick2 = "NICK astrid\n";
	string user2 = "USER astrid astrid_PC localhost :astrid geels\n";
	string mode2 = "MODE astrid +i\n";
	string ping2 = "PING localhost\n";


	test(env, capreq, fd_user1);
	test(env, nick1, fd_user1);
	test(env, user1, fd_user1);
	test(env, mode1, fd_user1);
	test(env, ping1, fd_user1);


	test(env, capreq, fd_user2);
	test(env, nick2, fd_user2);
	test(env, user2, fd_user2);
	test(env, mode2, fd_user2);
	test(env, ping2, fd_user2);

}

int main() {
	env env;
	int fd_user1 = 4;
	int fd_user2 = 5;
	env.server_address = "localhost";

	// Input:

	setup_test(env);

	string message4 = "WHOIS";
	string message5 = "WHOIS\n";
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

	// WHOIS
	test(env, message4, fd_user1);
	test(env, message5, fd_user1);
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
