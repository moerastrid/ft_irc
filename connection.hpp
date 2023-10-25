#pragma once

class connection {
private:
	int			fd;

public:
	connection();
	~connection();
	connection(int fd);
	connection(const connection& other);
	connection& operator=(const connection& other);
};
