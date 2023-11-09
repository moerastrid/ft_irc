#include "test.hpp"

void CAP_test() {
	int fd_user1 = 4;
	env env;
	Executor ex(env);

	string capreq = "CAP LS\n";
	test(ex, capreq, fd_user1, ":localhost CAP NAK :-\n");
}