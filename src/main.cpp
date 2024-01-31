/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 20:36:07 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:36:08 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

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
		cout << "Successfully booted the " << ircServer << " at " << env << " \\^.^/" << endl;
		
		while (sh.getInterrupted() == false) {
				ircServer.run(ex);
		}

		if (!env.getClients().empty()) {
			for (const auto &client : env.getClients()) {
				close(client.getFD());
			}
			env.getClients().clear();
		}
		if (!env.getChannels().empty())
			env.getChannels().clear();

		cout << "Successfully ended the " << ircServer << " \\^.^/" << endl;	
	} catch (const Server::ServerException &ex) {
		Msg("ServerException caught: ", "ERROR");
		cerr << ex.what() << endl;
		return (EXIT_FAILURE);
	}
	sh.setDefault();
	
	return (EXIT_SUCCESS);
}
