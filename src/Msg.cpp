/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Msg.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 14:57:33 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 14:57:34 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Msg.hpp"

void	Msg::structure(string text) {
	cout << "\x1B[35m" << "CLASS-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::debug(string text) {
	cout << "\x1B[36m" << "DEBUG-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::info(string text) {
	cout << "\x1B[34m" << "INFO-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::warning(string text) {
	cout << "\x1B[33m" << "WARNING-\t" << text << "\x1B[0m" << endl;
}
void	Msg::error(string text) {
	cout << "\x1B[31m" << "ERROR-\t\t" << text << "\x1B[0m" << endl;
}

Msg::Msg() {}
Msg::Msg(const Msg &src) {
	*this = src;
}
Msg	&Msg::operator=(const Msg &src) {
	(void)src;
	return (*this);
}

typedef void (Msg::*Msgptr)(void);

Msg::~Msg() { }
Msg::Msg(string text, string level) {
	void	(Msg::*Msgptr[])(string) = {&Msg::structure, &Msg::debug, &Msg::info, &Msg::warning, &Msg::error};
	string severity[] = {"CLASS", "DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i(0); i < 5; i++)
	{
		if (level.compare(severity[i]) == 0)
		{
			(this->*Msgptr[i])(text);
			return ;
		}
	}
}

