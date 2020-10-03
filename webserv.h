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
#include <sys/stat.h>
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

#define WEBSERV_VER "0.1"

//CONFIG PARSER
typedef struct conf
{
	std::vector<int> ports;
	std::string host;
	std::vector<std::string> indexs;
	size_t body_limit;
}	t_conf;
t_conf parseConf(std::string);

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
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
	std::string query;
	bool bad_request;//Allows bad_request checks before/while parsing request
}				t_req_line;

int parse_request(char *request, int fd, t_net&, t_conf);

//RESPONSE
typedef struct	s_http_res
{
	std::string http_ver;// ex : HTTP/1.1
	std::string status_code; // ex: 
	std::string reason_phrase; // GET, PUT, POST,...
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
}				t_http_res;
int answer_request(int client_fd, t_req_line rl, t_net &snet, t_conf conf);
//RESPONSE UTILS
int bad_request(t_req_line rl);
int valid_http_ver(t_req_line rl);
void handle_absolute_path(t_req_line &rl);
void parse_query_from_target(t_req_line &rl);


//DATE
std::string get_imf_fixdate();
std::string get_last_modified(std::string filename);
//CONTENT_TYPE
std::string get_content_type(std::string filename);
//STATUS_CODE
void send_400(t_req_line rl, t_http_res &resp);
void send_403(t_req_line rl, t_http_res &resp);
void send_404(t_req_line rl, t_http_res &resp);
void send_413(t_req_line rl, t_http_res &resp);
void send_505(t_req_line rl, t_http_res &resp);
void send_200(t_req_line rl, t_http_res &resp, int fd);

//UTILS
int print_err(std::string s);
void excerr(std::string msg, int c);
bool file_exists(std::string filename);

#endif