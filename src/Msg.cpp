/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Msg.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 14:57:33 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:35:36 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Msg.hpp"

void	Msg::structure([[maybe_unused]]const string text) {
	// cout << "\x1B[35m" << "CLASS-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::customStream([[maybe_unused]]const string text) {
	// cout << "\x1B[44m" << "STREAM-\t\t";
	// for (auto i : text) {
	// 	if (i == '\r') {
	// 		cout << "\\r";
	// 	} else if (i == '\n') {
	// 		cout << "\\n";
	// 	} else {
	// 		cout << i;
	// 	}
	// }
	// cout << "\033[0m" << endl;
}
void	Msg::debug([[maybe_unused]]const string text) {
	// cout << "\x1B[36m" << "DEBUG-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::info(const string text) {
	cout << "\x1B[34m" << "INFO-\t\t" << text << "\x1B[0m" << endl;
}
void	Msg::warning(const string text) {
	cout << "\x1B[33m" << "WARNING-\t" << text << "\x1B[0m" << endl;
}
void	Msg::error(const string text) {
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
Msg::Msg(const string text, const string level) {
	void	(Msg::*Msgptr[])(string) = {&Msg::structure, &Msg::customStream, &Msg::debug, &Msg::info, &Msg::warning, &Msg::error};
	string severity[] = {"CLASS", "STREAM", "DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i(0); i < 6; i++)
	{
		if (level.compare(severity[i]) == 0)
		{
			(this->*Msgptr[i])(text);
			return ;
		}
	}
	cout << "\x1B[31m" << "severity lvl not found for message-\t\t" << text << "\x1B[0m" << endl;
}

