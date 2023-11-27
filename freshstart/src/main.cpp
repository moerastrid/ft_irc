#include "ircserver.hpp"

int main(int argc, char **argv) {
	SignalHandler	sh;
	sh.setCustom();

	int	port = parse(argc, argv);
	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Env env(port, argv[2]);
	Executor	ex(env);


	try {
		Server ircServer(env);
		std::cout << "Successfully booted the " << ircServer << " \\^.^/" << std::endl;
		
		while (sh.getInterrupted() == false) {
				ircServer.run(ex);
		}

		if (!env.clients.empty()) {
			for (const auto &client : env.clients) {
				close(client.getFD());
			}
			env.clients.clear();
		}
		if (!env.channels.empty())
			env.channels.clear();

		std::cout << "Successfully ended the " << ircServer << " \\^.^/" << std::endl;	
	} catch (const Server::ServerException &ex) {
		Msg("ServerException caught: ", "ERROR");
		std::cerr << ex.what() << std::endl;
		return (EXIT_FAILURE);
	}
	sh.setDefault();
	
	return (EXIT_SUCCESS);
}
