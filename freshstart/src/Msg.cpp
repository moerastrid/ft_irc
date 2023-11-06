#include "Msg.hpp"

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

Msg::Msg() {}								//default constructor

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
	void	(Msg::*Msgptr[])(string) = {&Msg::debug, &Msg::info, &Msg::warning, &Msg::error};
	string severity[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i(0); i < 4; i++)
	{
		if (level.compare(severity[i]) == 0)
		{
			(this->*Msgptr[i])(text);
			return ;
		}
	}
}

