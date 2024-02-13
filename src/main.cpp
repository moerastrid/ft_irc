/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 20:36:07 by ageels        #+#    #+#                 */
/*   Updated: 2024/02/08 14:17:18 by ageels        ########   odam.nl         */
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
		std::stringstream ss1;
		ss1 << "Successfully booted the " << ircServer << " at " << env << " \\^.^/" << endl;
		Msg(ss1.str(), "INFO");
		while (sh.getInterrupted() == false) {
				ircServer.run(ex);
		}

		if (!env.getClients().empty()) {
			for (const auto &client : env.getClients()) {
				close(client->getFD());
			}
			//env.getClients().clear();
			env.clearClients();
		}
		if (!env.getChannels().empty())
			env.getChannels().clear();

		std::stringstream ss2;
		ss2 << "Successfully ended the " << ircServer << " \\^.^/" << endl;
		Msg(ss2.str(), "INFO");
	} catch (const Server::ServerException &ex) {
		Msg("ServerException caught: ", "ERROR");
		cerr << ex.what() << endl;
		return (EXIT_FAILURE);
	}
	sh.setDefault();
	
	return (EXIT_SUCCESS);
}
