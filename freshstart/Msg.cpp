#include "Msg.hpp"

void	Msg::debug(std::string text) {
	std::cout << "\x1B[36m" << "DEBUG-\t\t" << text << "\x1B[0m" << std::endl;
}

void	Msg::info(std::string text) {
	std::cout << "\x1B[34m" << "INFO-\t\t" << text << "\x1B[0m" << std::endl;
}

void	Msg::warning(std::string text) {
	std::cout << "\x1B[33m" << "WARNING-\t" << text << "\x1B[0m" << std::endl;
}

void	Msg::error(std::string text) {
	std::cout << "\x1B[31m" << "ERROR-\t\t" << text << "\x1B[0m" << std::endl;
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

Msg::Msg(std::string text, std::string level) {
	void	(Msg::*Msgptr[])(std::string) = {&Msg::debug, &Msg::info, &Msg::warning, &Msg::error};
	std::string severity[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i(0); i < 4; i++)
	{
		if (level.compare(severity[i]) == 0)
		{
			(this->*Msgptr[i])(text);
			return ;
		}
	}
}

