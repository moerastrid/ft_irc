#pragma once
#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

class Msg {
	private	:
		void	debug(string text);
		void	info(string text);
		void	warning(string text);
		void	error(string text);

		Msg();								//default constructor
		Msg(const Msg &src);			// copy constructor
		Msg	&operator=(const Msg &src);	// = sign operator
		typedef void (Msg::*Msgptr)(string);

	public	:
		~Msg();								// default destructor
		Msg(string text, string level);
};
