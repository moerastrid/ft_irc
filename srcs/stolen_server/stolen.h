#ifndef _BASE_H_
#define _BASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>

#include <ctype.h>

typedef struct t_user {
	char	*nick;
	char	*user;
	int		fd;
} s_user;

typedef struct sockaddr_storage sock_store;

typedef struct	t_env
{
	sock_store	their_addr;
	socklen_t	addr_size;
	int			newfd;
	int			sockfd;
	int			fdmax;
	fd_set		master;
	fd_set		fdreads;
	fd_set		fdwrites;
	s_user**	clients;
	int			active_clients;
	int			handshakes[256];
	char*		server_address;
}		s_env;

#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKCOLLISION 433

// Never ffing do this.
const char* errors[] = {
	[431] = "ERR_NONICKNAMEGIVEN",
	[432] = "ERR_ERRONEUSNICKNAME",
	[433] = "ERR_NICKCOLLISION"
	// Add more error codes and strings as needed
};

#endif