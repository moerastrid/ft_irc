#include "test.hpp"

void test(Executor& ex, string& incoming, int& fd, string expected) {
	static int i = 0;
	CustomOutputStream customOut(cout);

	cout << "Test " << i++ << endl;
	customOut << "Processing Incoming message: [" + incoming + "]" << endl;

	Command cmd(incoming);

	cout << "Reply received: ";
	string reply = ex.run(cmd, fd);

	if (reply.compare(expected) == 0) {
		customOut << COLOR_GREEN << "[" << reply << "]" << endl;
		cout << "Expected      : ";
		customOut << "[" << expected << "]" << COLOR_RESET << endl;
	} else {
		customOut << COLOR_RED << "[" << reply << "]" << endl;
		cout << "Expected      : ";
		customOut << "[" << expected << "]" << COLOR_RESET << endl;
	}

	cout << "===============================" << endl << endl;
}

void run(Executor& ex, string& incoming, int& fd) {
	Command cmd(incoming);
	string reply = ex.run(cmd, fd);
	CustomOutputStream customOut(cerr);
	customOut << "Reply: " << reply << endl;
	cerr << "===============================" << endl;
}

// Connects two clients to the server, and joins four test channels. 
// user1 is founder on channel 1, founder + operator on channel 2, joined to channel 3, and not on channel 4.
// User2 is founder on channel 4, founder + operator on channel 3, joined to channel 2, and not on channel 1.
// Setup otherwise manually.
// Variable name convention: join11 = command+userfd+channelnumber.
void connect_two_clients(Executor& ex) {
	// Reply format: :<server> <response_code | command> <caller> <target> [#<channel>] :<message>
	int fd_user1 = 4;
	int fd_user2 = 5;

	string capreq = "CAP LS\n";
	run(ex, capreq, fd_user1);

	string nick1 = "NICK neus\n";
	string user1 = "USER Thibauld Thibauld_PC localhost :Thibauld WW Nuyten\n";
	string mode1 = "MODE neus +i\n";
	string whois1 = "WHOIS neus\n";
	string ping1 = "PING localhost\n";
	run(ex, nick1, fd_user1);
	run(ex, user1, fd_user1);
	run(ex, mode1, fd_user1);
	run(ex, whois1, fd_user1);
	run(ex, ping1, fd_user1);

	string nick2 = "NICK astrid\n";
	string user2 = "USER Astrid Astrid_PC localhost :Astrid Geels\n";
	string mode2 = "MODE astrid +i\n";
	string whois2 = "WHOIS astrid\n";
	string ping2 = "PING localhost\n";
	run(ex, capreq, fd_user2);
	run(ex, user2, fd_user2);
	run(ex, nick2, fd_user2);
	run(ex, mode2, fd_user2);
	run(ex, whois2, fd_user2);
	run(ex, ping2, fd_user2);

	string join11 = "JOIN #test 123";
	string join12 = "JOIN #test2";
	run(ex, join11, fd_user1);
	run(ex, join12, fd_user1);

	string join24 = "JOIN #test4 234";
	string join23 = "JOIN #test3";
	run(ex, join24, fd_user2);
	run(ex, join23, fd_user2);

	string join22 = "JOIN #test2";
	string join21 = "JOIN #test 123";
	run(ex, join22, fd_user2);
	run(ex, join21, fd_user2);

	string join13 = "JOIN #test3";
	string join14 = "JOIN #test4 234";
	run(ex, join13, fd_user1);
	run(ex, join14, fd_user1);
}

int main() {
	// int fd_user1 = 4;
	// int fd_user2 = 5;

	CAP_test();
	PASS_test();
	NICK_test();
	USER_test();
	MODE_test();
	PING_test();
	PRIVMSG_test();
	WHOIS_test();
	JOIN_test();
	KICK_test();
	PART_test();
	INVITE_test();
	TOPIC_test();
	QUIT_test();


	// // WHOIS
	// string message4 = "WHOIS\n";
	// string message5 = "WHOIS 123 :zlsd:13'!@#$%^&*()\n";
	// string message6 = "WHOIS neus astrid doofus\n";
	// test(env, message4, fd_user2, ":localhost 311 astrid astrid Astrid Astrid_PC * :Astrid Geels\n");
	// test(env, message5, fd_user1, ":localhost 401 neus 123 :No such nickname\n:localhost 401 neus :zlsd:13'!@#$%^&*() :No such nickname\n");
	// test(env, message6, fd_user2, ":localhost 311 astrid neus Thibauld Thibauld_PC * :Thibauld WW Nuyten\n:localhost 311 astrid astrid Astrid Astrid_PC * :Astrid Geels\n:localhost 401 astrid doofus :No such nickname\n");

	// // JOIN test.
	// string joinmessage1 = "JOIN #test,#test2,#test3 123,234\n"; // RPL_TOPIC
	// string joinmessage2 = "JOIN #test3 234\n"; 					// RPL_TOPIC Success with superfluous password.
	// string joinmessage3 = "JOIN #test2 qwerty\n"; 				// ERR_BADCHANNELKEY
	// string joinmessage4 = "JOIN #test2 234\n"; 					// ERR_USERONCHANNEL
	// string joinmessage5 = "JOIN \n"; 							// ERR_NEEDMOREPARAMS
	// string joinmessage6 = "JOIN 1 1 1 1 1 1 1 1\n"; 			// ERR_TOOMANYPARAMS
	// test(env, joinmessage1, fd_user1, ":localhost 332 neus #test :Welcome to channel #test\n:localhost 332 neus #test2 :Welcome to channel #test2\n:localhost 332 neus #test3 :Welcome to channel #test3\n");
	// test(env, joinmessage2, fd_user2, ":localhost 332 astrid #test3 :Welcome to channel #test3\n");
	// test(env, joinmessage3, fd_user2, ":localhost 475 astrid #test2 :Bad channel key\n");
	// test(env, joinmessage4, fd_user1, ":localhost 443 neus #test2 :Cannot join channel - you are already on the channel\n");
	// test(env, joinmessage5, fd_user1, ":localhost 461 neus JOIN :Not enough parameters\n");
	// test(env, joinmessage6, fd_user1, ":localhost 461 neus JOIN :Too many parameters\n");

	// // KICK test
	// string kickmessage1 = "KICK \n";
	// string kickmessage2 = "KICK #test neus :go away\n";
	// string kickmessage3 = "KICK #test neus :go away\n";
	// test(env, kickmessage1, fd_user1, ":localhost 461 neus KICK :Not enough parameters\n");
	// test(env, kickmessage2, fd_user1, ":localhost KICK #test neus :go away\n");
	// test(env, kickmessage3, fd_user1, ":localhost 441 neus neus #test :Cannot kick user from a channel that they have not joined\n");
	
	// // PART test
	// string partmessage0 = "PART\n";
	// string partmessage1 = "PART #test2 :you guys suck\n";
	// string partmessage2 = "PART #test2,#test3 :screw you guys, I'm going home\n";
	// test(env, partmessage0, fd_user1, ":localhost 461 neus PART :Not enough parameters\n");
	// test(env, partmessage1, fd_user1, ":localhost neus PART #test2\n");
	// test(env, partmessage2, fd_user1, ":localhost PART #\n");

	// for (const auto& el : env.clients) {
	// 	cout << el << endl;
	// }
}
