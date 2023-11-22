#include "ircserver.hpp"

#include "Executor.hpp"
#include "env.hpp"

int main(int argc, char **argv) {
	int			port;
	env env;
	Executor	ex(env);

	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	
	Server ircServer(port, argv[2]);
	std::cout << "Successfully booted the " << ircServer << " \\^.^/" << std::endl;
	
	while (1) {
		ircServer.run(ex);
	}
	std::cout << "Successfully ended the " << ircServer << " \\^.^/" << std::endl;
	return (EXIT_SUCCESS);
}
