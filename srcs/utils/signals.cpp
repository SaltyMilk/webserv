#include "../includes/webserv.h"

void	chandler(int sig_num)
{
    signal(SIGINT, SIG_IGN);
	exit(1);
	(void)sig_num;
}
