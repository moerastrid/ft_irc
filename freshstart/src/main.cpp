#include "ircserver.hpp"

int main(int argc, char **argv) {
	int	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Env env(port, argv[2]);
	Executor	ex(env);

	Server ircServer(env);
	std::cout << "Successfully booted the " << ircServer << " \\^.^/" << std::endl;
	


	SignalHandler	sh;
	sh.setCustom();

	while (sh.getInterrupted() == false) {
		ircServer.run(ex);
	}
	if (!env.clients.empty())
		env.clients.clear();
	if (!env.channels.empty())
		env.channels.clear();
	std::cout << "Successfully ended the " << ircServer << " \\^.^/" << std::endl;
	sh.setDefault();
	
	return (EXIT_SUCCESS);
}
