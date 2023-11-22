#include "Env.hpp"

Env::Env(int port, string pass) : pass(pass) {
	memset(&this->sockin, 0, sizeof(this->sockin));
	memset(&this->sockfd, 0, sizeof(this->sockfd));

	this->sockfd.events = POLLIN | POLLHUP;
	this->sockin.sin_family = AF_INET;
	this->sockin.sin_port = htons(port);
	this->sockin.sin_addr.s_addr = INADDR_ANY; /*perhaps htonl(INADDR_ANY); instead?*/
}