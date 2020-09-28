#ifndef WEBSERV_H
#define WEBSERV_H
//GENERAL INCLUDES
#include <string>
#include <list>
#include <map>
#include <vector>
#include <errno.h>
#include <iostream>
#include "libft/libft.h"
#include <fcntl.h>
#include <unistd.h>
//NETWORK INCLUDES
#include <stdlib.h>
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
//DATE INCLUDES
#include <sys/time.h>

//HEADER DEFINES
#define ACCEPT_CHARSETS 	0
#define ACCEPT_LANGUAGE 	1
#define ALLOW				2
#define AUTHORIZATION		3
#define CONTENT_LANGUAGE	4
#define CONTENT_LENGTH		5
#define CONTENT_LOCATION	6
#define CONTENT_TYPE		7
#define DATE				8
#define HOST				9
#define LAST_MODIFIED		10
#define LOCATION			11
#define REFERER				12
#define RETRY_AFTER			13
#define SERVER				14
#define TRANSFER_ENCODING	15
#define USER_AGENT			16
#define WWW_AUTHENTICATE	17

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
	std::map<std::string, std::string> headers; //each fieldname :: field-value
	std::string body;
}				t_req_line;

int parse_request(char *request, int fd, t_net&);

//RESPONSE
typedef struct	s_http_res
{
	std::string http_ver;// ex : HTTP/1.1
	std::string status_code; // ex: 
	std::string reason_phrase; // GET, PUT, POST,...
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
}				t_http_res;
int answer_request(int client_fd, t_req_line rl, t_net &snet);

//CONFIG PARSER
typedef struct conf
{
	std::vector<int> ports;
	std::string host;
}	t_conf;
t_conf parseConf(std::string);

//DATE
std::string get_imf_fixdate();
//CONTENT_TYPE
std::string get_content_type(std::string filename);

//UTILS
int print_err(std::string s);
void excerr(std::string msg, int c);

#endif