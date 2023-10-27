#pragma once
#include <string>
#include <iostream>

class Msg {
	private	:
		void	debug(std::string text);
		void	info(std::string text);
		void	warning(std::string text);
		void	error(std::string text);

		Msg();								//default constructor
		Msg(const Msg &src);			// copy constructor
		Msg	&operator=(const Msg &src);	// = sign operator
		typedef void (Msg::*Msgptr)(std::string);

	public	:
		~Msg();								// default destructor
		Msg(std::string text, std::string level);
};
