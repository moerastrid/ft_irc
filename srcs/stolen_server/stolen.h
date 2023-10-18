#ifndef _BASE_H_
#define _BASE_H_

typedef struct t_user {
	char	*name;
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
}		s_env;

#endif