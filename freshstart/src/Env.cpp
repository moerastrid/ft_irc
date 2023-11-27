#include "Env.hpp"

Env::Env(int port, string pass) : pass(pass), port(port) {};

const Client& Env::getClientByFD(int fd) const {
	for(const auto& el : this->clients) {
		if (el.getPFD().fd == fd)
			return el;
	}

	return Client::nullclient;
}

vector<Client>& Env::getClients() {
	return this->clients;
}