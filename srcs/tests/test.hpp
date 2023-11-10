#ifndef TEST_HPP
#define TEST_HPP

#include "../Executor.hpp"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <ostream>
using std::ostream;

#define COLOR_RED	"\x1b[31m"
#define COLOR_GREEN	"\x1b[32m"

// Define ANSI escape code to reset text color to the default
#define COLOR_RESET	"\x1b[0m"

class CustomOutputStream : public ostream {
public:
	CustomOutputStream(ostream& output) : ostream(output.rdbuf()), output_stream(output) {}

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
	CustomOutputStream& operator<<(ostream& (*manipulator)(ostream&)) {
		manipulator(output_stream);
		return *this;
	};

private:
	ostream& output_stream;
};

void test(Executor& ex, string incoming, int& fd, string expected);
void run(Executor& ex, string incoming, int& fd);
void connect_two_clients(Executor& ex);

void CAP_test();
void PASS_test();
void NICK_test();
void USER_test();
void MODE_test();
void PING_test();
void PRIVMSG_test();
void WHOIS_test();
void JOIN_test();
void KICK_test();
void PART_test();
void INVITE_test();
void TOPIC_test();
void QUIT_test();

#endif
