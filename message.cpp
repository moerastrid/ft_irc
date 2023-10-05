#include "message.hpp"

void	message::debug(std::string text) {
	std::cout << "\x1B[36m" << "DEBUG- " << text << "\x1B[0m" << std::endl;
}

void	message::info(std::string text) {
	std::cout << "\x1B[35m" << "INFO- " << text << "\x1B[0m" << std::endl;
}

void	message::warning(std::string text) {
	std::cout << "\x1B[34m" << "WARNING- " << text << "\x1B[0m" << std::endl;
}

void	message::error(std::string text) {
	std::cout << "\x1B[33m" << "ERROR- " << text << "\x1B[0m" << std::endl;
}

message::message() {}								//default constructor

message::message(const message &src) {
	*this = src;
}

message	&message::operator=(const message &src) {
	(void)src;
	return (*this);
}

typedef void (message::*messageptr)(void);

message::~message() { }

message::message(std::string text, std::string level) {
	void	(message::*messageptr[])(std::string) = {&message::debug, &message::info, &message::warning, &message::error};
	std::string severity[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i(0); i < 4; i++)
	{
		if (level.compare(severity[i]) == 0)
		{
			(this->*messageptr[i])(text);
			return ;
		}
	}
}

