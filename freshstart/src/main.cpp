#include "ircserver.hpp"

#include "Executor.hpp"
#include "Env.hpp"

int main(int argc, char **argv) {
	int	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Env env(port, argv[2]);
	Executor	ex(env);

	Server ircServer(env);
	std::cout << "Successfully booted the " << ircServer << " \\^.^/" << std::endl;
	
	while (1) {
		ircServer.run(ex);
	}
	std::cout << "Successfully ended the " << ircServer << " \\^.^/" << std::endl;
	return (EXIT_SUCCESS);
}
