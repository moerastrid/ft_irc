/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SignalHandler.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 20:25:50 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:25:51 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "SignalHandler.hpp"

SignalHandler::SignalHandler() {}
SignalHandler::~SignalHandler() {}

bool SignalHandler::interrupted = false;

void	SignalHandler::setDefault(void) {
	signal(SIGINT, SIG_DFL);
}
void	SignalHandler::setCustom(void) {
	signal(SIGINT, &SignalHandler::sigintHandler);
}
bool	SignalHandler::getInterrupted() {
	return (interrupted);
}
