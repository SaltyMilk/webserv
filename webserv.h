#ifndef WEBSERV_H
#define WEBSERV_H
//GENERAL INCLUDES
#include <string>
#include <list>
#include <errno.h>
#include <iostream>
#include "libft/libft.h"
#include <fcntl.h>
#include <unistd.h>
//NETWORK INCLUDES
#include <stdlib.h>
#include <map>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>



//NET
typedef struct	s_net
{
	std::list<int> client_fds;
}				t_net;

//PARSER
typedef struct	s_request_line
{
	std::string method; // GET, PUT, POST,...
	std::string target; // ex: /index, http://https://profile.intra.42.fr/,...
	std::string http_ver;// ex : HTTP/1.1
	std::map<std::string, std::string> field; //each fieldname :: field-value
}				t_req_line;

int parse_request(char *request, int fd, t_net&);

//UTILS
int print_err(std::string s);

#endif