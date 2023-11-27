#include "SignalHandler.hpp"

// void SignalHandler::sigintHandler(int signum) {
// 	std::cout << "Interrupted signal (" << signum << ") received";
// }

bool SignalHandler::interrupted = false;

void	SignalHandler::setCustom(void) {
	signal(SIGINT, &SignalHandler::sigintHandler);
}

void	SignalHandler::setDefault(void) {
	signal(SIGINT, SIG_DFL);
}

SignalHandler::SignalHandler()
{
	std::cout << "hey\n";
}

SignalHandler::~SignalHandler()
{
}

bool	SignalHandler::getInterrupted() {
	return (interrupted);
}