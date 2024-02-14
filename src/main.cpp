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
	int	exit_status = EXIT_SUCCESS;
	int	port = parse(argc, argv);

	if (port < 0) {
		return (EXIT_FAILURE);
	}

	Env				env(port, argv[2]);
	Executor		exec(env);
	SignalHandler	sh;

	sh.setCustom();
	try {
		Server ircServer(env);
		while (sh.getInterrupted() == false)
			ircServer.run(exec);
	} catch (const Server::ServerException &exception) {
		Msg("ServerException caught: ", "ERROR");
		cerr << exception.what() << endl;
		exit_status = EXIT_FAILURE;
	}
	env.reset();
	sh.setDefault();
	return (exit_status);
}
