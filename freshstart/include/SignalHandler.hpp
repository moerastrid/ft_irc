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
			// std::cout << "Interrupted signal (" << signum << ") received\n";
			interrupted = true;
		}
		
	public:
		SignalHandler();
		~SignalHandler();
		void	setDefault(void);
		void	setCustom(void);
		bool	getInterrupted(void);
};
