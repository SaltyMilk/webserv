#ifndef PARSER_H
#define PARSER_H
#include "webserv.h"
typedef struct	s_request_line
{
	std::string method; // GET, PUT, POST,...
	std::string target; // ex: /index, http://https://profile.intra.42.fr/,...
	std::string http_ver;// ex : HTTP/1.1
}				t_req_line;

int parse_request(char *request, int fd);

#endif