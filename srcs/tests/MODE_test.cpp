#include "test.hpp"

//void test_num_param(Executor& ex) { // Elsewhere?
//	int fd_user1 = 4;
//
//	test(ex, "MODE \n", fd_user1, "\n");
//}

void test_invalid_name(Executor& ex) { // 403 ERR_NOSUCHCHANNEL
	int fd_user1 = 4;

	test(ex, "MODE doesn'texist +i \n", fd_user1, ":localhost 403 neus doesn'texist :No such channel (we don't support changing modes for users)\n");
	test(ex, "MODE doesntexist +o neus\n", fd_user1, ":localhost 403 neus doesntexist :No such channel (we don't support changing modes for users)\n");
	test(ex, "MODE doesntexist +k pass\n", fd_user1, ":localhost 403 neus doesntexist :No such channel (we don't support changing modes for users)\n");
	test(ex, "MODE doesntexist +t bla bla\n", fd_user1, ":localhost 403 neus doesntexist :No such channel (we don't support changing modes for users)\n");
	test(ex, "MODE doesntexist +l 123\n", fd_user1, ":localhost 403 neus doesntexist :No such channel (we don't support changing modes for users)\n");

	test(ex, "MODE #doesntexist +i \n", fd_user1, ":localhost 403 neus #doesntexist :No such channel\n");
	test(ex, "MODE #doesntexist +o neus\n", fd_user1, ":localhost 403 neus #doesntexist :No such channel\n");
	test(ex, "MODE #doesntexist +k pass\n", fd_user1, ":localhost 403 neus #doesntexist :No such channel\n");
	test(ex, "MODE #doesntexist +t bla bla\n", fd_user1, ":localhost 403 neus #doesntexist :No such channel\n");
	test(ex, "MODE #doesntexist +l 123\n", fd_user1, ":localhost 403 neus #doesntexist :No such channel\n");
}

void test_rpl_channelmodeis(Executor& ex) { //324 RPL_CHANNELMODEIS
	int fd_user1 = 4;
	test(ex, "MODE #test\n", fd_user1, ":localhost 324 neus #test +l-itk 0\n");
}

void test_basic_modes(Executor& ex) {
	int fd_user1 = 4;

	test(ex, "MODE #test2 +i\n", fd_user1, ":localhost 324 neus #test2 +i neus\n");
	test(ex, "MODE #test2 +t\n", fd_user1, ":localhost 324 neus #test2 +t \n");
	test(ex, "MODE #test2 +k password\n", fd_user1, ":localhost 324 neus #test2 +k neus\n");
	test(ex, "MODE #test2 +o neus\n", fd_user1, ":localhost 324 neus #test2 +o neus\n");
	test(ex, "MODE #test2 +l 10\n", fd_user1, ":localhost 324 neus #test2 +l neus\n");
}

void MODE_test() {
	//setup
	env env;
	env.server_address = "localhost";
	Executor ex(env);
	connect_two_clients(ex);

	//tests
	test_invalid_name(ex);
	test_rpl_channelmodeis(ex);
	test_basic_modes(ex);
}
