#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

using std::cout;

void server() {
	// create socket
	const int	port = 8080;
	// later port van cmd line
	const int	max_connections = 4;
	int 		server_fd;
	int 		connection;
	sockaddr_in	address;

	// server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "cannot create socket" << std::endl;
	} else {
		cout << "socket " << server_fd << " created" << std::endl;
	}

	// bind socket
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_fd, ((const sockaddr *)&address), sizeof(address)) < 0) {
		std::cerr << "Cannot bind socket" << std::endl;
		return ;
	} else
		std::cout << "socket " << server_fd << " bind success" << std::endl;

	// wait for connection
	if (listen(server_fd, max_connections) < 0) {
		std::cerr << "Cannot hear socket" << std::endl;
		return ;
	} else
		std::cout << "socket " << server_fd << " loud and clear over" << std::endl;
	
	socklen_t len = sizeof(address);

	// begin loop? voor meerdere connections
	while (1) {
		connection = accept(server_fd, (struct sockaddr *)&address, &len);
		if (connection < 0) {
			// std::cerr << "Cannot accept socket" << std::endl;
				(void)connection;
			} else {
			std::cout << "socket " << server_fd << " accepted" << std::endl;

			// receive info
			char buf[40000];
			long valread = read(connection, buf, 40000);
			std::cout << buf << std::endl;
			std::cout << "valread : " << valread << std::endl;

			// send message
			std::string response = "this is FT_IRC over \n";
			
			send(connection, response.c_str(), response.size(), 0);
			std::cout << "hello message sent" << std::endl;
			}
		// pas helemaal als je klaar bent:
		close(connection);
	}
	close(server_fd);
}