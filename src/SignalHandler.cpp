#include "SignalHandler.hpp"

bool SignalHandler::interrupted = false;

void	SignalHandler::setCustom(void) {
	signal(SIGINT, &SignalHandler::sigintHandler);
}

void	SignalHandler::setDefault(void) {
	signal(SIGINT, SIG_DFL);
}

SignalHandler::SignalHandler() {}

SignalHandler::~SignalHandler() {}

bool	SignalHandler::getInterrupted() {
	return (interrupted);
}
