#include "connection.hpp"

connection::connection() {
	this->fd = -1;
}

connection::~connection() {}

connection::connection(const connection& src) {
	*this = src;
}

connection& connection::operator=(const connection& src) {
	this->fd = src.fd;
	return *this;
}
