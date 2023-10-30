#pragma once
#include <string>
#include <iostream>

class message {
	private	:
		void	debug(std::string text);
		void	info(std::string text);
		void	warning(std::string text);
		void	error(std::string text);

		message();								//default constructor
		message(const message &src);			// copy constructor
		message	&operator=(const message &src);	// = sign operator
		typedef void (message::*messageptr)(std::string);

	public	:
		~message();								// default destructor
		message(std::string text, std::string level);
};
