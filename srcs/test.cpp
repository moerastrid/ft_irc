#include "Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;

#define COLOR_RED	"\x1b[31m"
#define COLOR_GREEN	"\x1b[32m"

// Define ANSI escape code to reset text color to the default
#define COLOR_RESET	"\x1b[0m"

class CustomOutputStream : public std::ostream {
public:
	CustomOutputStream(std::ostream& output) : std::ostream(output.rdbuf()), output_stream(output) {}

	template <typename T>
	CustomOutputStream& operator<<(const T& value) {
		for(const auto& el : value) {
			if (el == '\n') {
				// Special handling for printing the literal "\n"
				output_stream << "\\n";
			} else {
				output_stream << el;
			}
		}
		return *this;
	};

	// Override the << operator for endl
	CustomOutputStream& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
		manipulator(output_stream);
		return *this;
	};

private:
	std::ostream& output_stream;
};

void test(env& env, string& incoming, int& fd, string expected) {
	static int i = 0;
	CustomOutputStream customOut(std::cout);
	
	
	static Executor ex(env);

	cout << "Test " << i++ << endl;
	customOut << "Processing Incoming message: [" + incoming + "]" << std::endl;

	Command cmd(incoming, fd);

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


void CAP_test(env& env) {
	int fd_user1 = 4;
//	int fd_user2 = 5;

	string capreq = "CAP LS\n";
	test(env, capreq, fd_user1, ":localhost CAP NAK :-\n");
}

void PASS_test(env& env) {
(void)env;
}

void NICK_test(env& env) {
	(void)env;
}

void USER_test(env& env) {
	(void)env;
}

void MODE_test(env& env) {
	(void)env;
}

void PING_test(env& env) {
	(void)env;
}

void PRIVMSG_test(env& env) {
	(void)env;
}

void WHOIS_test(env& env) {
	(void)env;
}

void JOIN_test(env& env) {
	(void)env;
}

void KICK_test(env& env) {
	(void)env;
}

void PART_test(env& env) {
	(void)env;
}

void INVITE_test(env& env) {
	(void)env;
}

void TOPIC_test(env& env) {
	(void)env;
}

void QUIT_test(env& env) {
	(void)env;
}

void connect_two_clients(env& env) {
	int fd_user1 = 4;
	int fd_user2 = 5;

	string capreq = "CAP LS\n";

	string nick1 = "NICK neus\n";
	string user1 = "USER Thibauld Thibauld_PC localhost :Thibauld WW Nuyten\n";
	string mode1 = "MODE neus +i\n";
	string whois1 = "WHOIS neus\n";
	string ping1 = "PING localhost\n";

	string nick2 = "NICK astrid\n";
	string user2 = "USER Astrid Astrid_PC localhost :Astrid Geels\n";
	string mode2 = "MODE astrid +i\n";
	string whois2 = "WHOIS astrid\n";
	string ping2 = "PING localhost\n";

	// Reply format: :<server> <response_code | command> <caller> <target> [#<channel>] :<message>
	test(env, capreq, fd_user1, ":localhost CAP NAK :-\n");
	test(env, nick1, fd_user1, ":localhost NOTICE neus neus :Remember to set your username using the USER command\n");
	test(env, user1, fd_user1, ":localhost 001 neus Thibauld :Welcome to Astrid's & Thibauld's IRC server, Thibauld!\n");
	test(env, mode1, fd_user1, ":localhost 403 neus neus :No such channel\n");
	test(env, whois1, fd_user1, ":localhost \n");
	test(env, ping1, fd_user1, "PONG localhost\n");

	test(env, capreq, fd_user2, ":localhost CAP NAK :-\n");
	test(env, user2, fd_user2, ":localhost NOTICE Astrid Astrid :Remember to set your nickname using the NICK command\n");
	test(env, nick2, fd_user2, ":localhost 001 astrid astrid :Welcome to Astrid's & Thibauld's IRC server, Astrid!\n");
	test(env, mode2, fd_user2, ":localhost 403 astrid astrid :No such channel\n");
	test(env, whois2, fd_user2, ":localhost \n");
	test(env, ping2, fd_user2, "PONG localhost\n");
}

int main() {
	env env;
	int fd_user1 = 4;
	int fd_user2 = 5;
	env.server_address = "localhost";

	// Input:
	connect_two_clients(env);

	// WHOIS
	string message4 = "WHOIS\n";
	string message5 = "WHOIS 123 :zlsd:13'!@#$%^&*()\n";
	string message6 = "WHOIS neus astrid doofus\n";
	test(env, message4, fd_user2, ":localhost 311 astrid astrid Astrid Astrid_PC * :Astrid Geels\n");
	test(env, message5, fd_user1, ":localhost 401 neus 123 :No such nickname\n:localhost 401 neus :zlsd:13'!@#$%^&*() :No such nickname\n");
	test(env, message6, fd_user2, ":localhost 311 astrid neus Thibauld Thibauld_PC * :Thibauld WW Nuyten\n:localhost 311 astrid astrid Astrid Astrid_PC * :Astrid Geels\n:localhost 401 astrid doofus :No such nickname\n");

	// JOIN test.
	string joinmessage1 = "JOIN #test,#test2,#test3 123,234\n"; // RPL_TOPIC
	string joinmessage2 = "JOIN #test3 234\n"; 					// RPL_TOPIC Success with superfluous password.
	string joinmessage3 = "JOIN #test2 qwerty\n"; 				// ERR_BADCHANNELKEY
	string joinmessage4 = "JOIN #test2 234\n"; 					// ERR_USERONCHANNEL
	string joinmessage5 = "JOIN \n"; 							// ERR_NEEDMOREPARAMS
	string joinmessage6 = "JOIN 1 1 1 1 1 1 1 1\n"; 			// ERR_TOOMANYPARAMS
	test(env, joinmessage1, fd_user1, ":localhost 332 neus #test :Welcome to channel #test\n:localhost 332 neus #test2 :Welcome to channel #test2\n:localhost 332 neus #test3 :Welcome to channel #test3\n");
	test(env, joinmessage2, fd_user2, ":localhost 332 astrid #test3 :Welcome to channel #test3\n");
	test(env, joinmessage3, fd_user2, ":localhost 475 astrid #test2 :Bad channel key\n");
	test(env, joinmessage4, fd_user1, ":localhost 443 neus #test2 :Cannot join channel - you are already on the channel\n");
	test(env, joinmessage5, fd_user1, ":localhost 461 neus JOIN :Not enough parameters\n");
	test(env, joinmessage6, fd_user1, ":localhost 461 neus JOIN :Too many parameters\n");

	// KICK test
	string kickmessage1 = "KICK \n";
	string kickmessage2 = "KICK #test neus :go away\n";
	string kickmessage3 = "KICK #test neus :go away\n";
	test(env, kickmessage1, fd_user1, ":localhost 461 neus KICK :Not enough parameters\n");
	test(env, kickmessage2, fd_user1, ":localhost KICK #test neus :go away\n");
	test(env, kickmessage3, fd_user1, ":localhost 441 neus neus #test :Cannot kick user from a channel that they have not joined\n");
	
	// PART test
	string partmessage0 = "PART\n";
	string partmessage1 = "PART #test2 :you guys suck\n";
	string partmessage2 = "PART #test2,#test3 :screw you guys, I'm going home\n";
	test(env, partmessage0, fd_user1, ":localhost 461 neus PART :Not enough parameters\n");
	test(env, partmessage1, fd_user1, ":localhost neus PART #test2\n");
	test(env, partmessage2, fd_user1, ":localhost PART #\n");

	for (const auto& el : env.clients) {
		cout << el << endl;
	}
}
