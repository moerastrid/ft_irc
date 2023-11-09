#include "test.hpp"

void MODE_test() {
	//setup
	int fd_user1 = 4;
	// int fd_user2 = 5;
	env env;
	env.server_address = "localhost";
	Executor ex(env);
	connect_two_clients(ex);

	//tests
	string m1 = "MODE #test +i";
	string m2 = "MODE #test +t neus";
	string m3 = "MODE #test +k password";
	string m4 = "MODE #test +o neus";
	string m5 = "MODE #test +l 10";
	test (ex, m1, fd_user1, ":localhost 324 neus #test +o neus\n");
}