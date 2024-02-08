/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SignalHandler.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 20:25:54 by ageels        #+#    #+#                 */
/*   Updated: 2024/02/08 14:17:28 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <csignal>
#include "Msg.hpp"

class SignalHandler
{
	private:
		static bool	interrupted;

		static void	sigintHandler(int signum){
			Msg("Interrupted signal (" + std::to_string(signum) + ") received", "WARNING");
			interrupted = true;
		}
		
	public:
		SignalHandler();
		~SignalHandler();

		void	setDefault(void);
		void	setCustom(void);
		bool	getInterrupted(void);
};
