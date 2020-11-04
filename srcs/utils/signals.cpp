#include "../../includes/webserv.h"

void	chandler(int sig_num)
{
    signal(SIGINT, SIG_IGN);
    for (std::vector<int>::iterator it = serv_socket.begin(); it != serv_socket.end(); it++)
    	close(*it);
	exit(1);
	(void)sig_num;
}
