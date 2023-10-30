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

void test(env& env, string& incoming, int fd, string expected) {
	CustomOutputStream customOut(std::cout);
	static Executor ex(env);

	customOut << "Processing Incoming message: [" + incoming + "]" << std::endl;

	Command cmd(incoming, fd);

	cout << "Reply received: ";
	string reply = ex.run(cmd, fd);

	if (reply.compare(expected) == 0) {
		customOut << COLOR_GREEN << "[" << reply << "]" << COLOR_RESET << endl;
	} else {
		customOut << COLOR_RED << "[" << reply << "]" << COLOR_RESET << endl;
	}
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


	test(env, capreq, fd_user1, "localhost CAP NAK :-\n");
	test(env, nick1, fd_user1, ":localhost NOTICE neus :Remember to set your username using the USER command\n");
	test(env, user1, fd_user1, ":localhost 001 neus :Welcome to Astrid's & Thibauld's IRC server, thibauld!\n");
	test(env, mode1, fd_user1, ":localhost 403 neus thibauld :No such channel");
	test(env, ping1, fd_user1, "PONG localhost");


	test(env, capreq, fd_user2, "localhost CAP NAK :-\n");
	test(env, nick2, fd_user2, ":localhost NOTICE astrid :Remember to set your username using the USER command.\n");
	test(env, user2, fd_user2, ":localhost 001 astrid :Welcome to Astrid's & Thibauld's IRC server, astrid!\n");
	test(env, mode2, fd_user2, ":localhost 403 astrid astrid :No such channel");
	test(env, ping2, fd_user2, "PONG localhost");

}

int main() {
	env env;
	int fd_user1 = 4;
	// int fd_user2 = 5;
	env.server_address = "localhost";

	// Input:

	setup_test(env);

	string message4 = "WHOIS";
	string message5 = "WHOIS\n";
	string message6 = "WHOIS neus astrid doofus\n";

	string message7 = "JOIN #test,#test2,#test3,#test4 123,234,345\n";
	string message8 = "JOIN #test3 qwerty\n";
	string message9 = "JOIN #test4\n";
	string message10 = "JOIN \n";

	string message11 = "KICK \n";
	string message12 = "KICK #test neus :go away\n";
	string message13 = "KICK #test neus :go away\n";

	string message14 = "JOIN #test 123\n";
	string message15 = "PART #test :you guys suck\n";

	// WHOIS
	test(env, message4, fd_user1, ":localhost 311 neus neus thibauld thibauld_PC * :Thibauld WW Nuyten\n");
	test(env, message5, fd_user1, ":localhost 311 neus neus thibauld thibauld_PC * :Thibauld WW Nuyten\n");
	test(env, message6, fd_user1, ":localhost 311 neus neus thibauld thibauld_PC * :Thibauld WW Nuyten\n:localhost 311 neus astrid astrid astrid_PC * :astrid geels\n:localhost 401 doofus :No such nickname\n");

	// JOIN test
	test(env, message7, fd_user1, ":localhost neus JOIN #test\n:localhost neus JOIN #test2\n:localhost neus JOIN #test3\n:localhost neus JOIN #test4\n");
	test(env, message8, fd_user1, ":localhost 475 neus #test3 :Bad channel key\n");
	test(env, message9, fd_user1, ":localhost neus JOIN #test4\n");
	test(env, message10, fd_user1, "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n");

	// KICK test
	test(env, message11, fd_user1, "461 ERR_NEEDMOREPARAMS JOIN :Not enough parameters\n");
	test(env, message12, fd_user1, ":localhost KICK #test neus :go away\n");
	test(env, message13, fd_user1, ":localhost 404 #test neus :Cannot kick user from channel they have not joined\n");
	
	// PART test
	string partmessage0 = "JOIN #test,#test2,#test3 123,234,345\n";
	string partmessage1 = "PART #test :you guys suck\n";
	string partmessage2 = "PART #test2,#test3 :screw you guys, I'm going home";

	test(env, partmessage0, fd_user1, ":localhost neus JOIN #test\n:localhost neus JOIN #test2\n:localhost neus JOIN #test3\n");
	test(env, partmessage1, fd_user1, ":localhost PART #test\n");
	test(env, partmessage2, fd_user1, ":localhost PART #\n");

	for (const auto& el : env.clients) {
		cout << el << endl;
	}
}
