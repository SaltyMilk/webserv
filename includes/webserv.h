#ifndef WEBSERV_H
#define WEBSERV_H
//GENERAL INCLUDES
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <errno.h>
#include <iostream>
#include "../libft/libft.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
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

#define BUFF_SIZE 4200

//HEADER DEFINES, also defines the order in which headers are sent
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

//Number of error status code supported
#define N_ERR_IMPLEMENTED 7
//Defines to be used with conf::default_error
#define ERR400 0
#define ERR403 1
#define ERR404 2
#define ERR405 3
#define ERR413 4
#define ERR501 5
#define ERR505 6

#define WEBSERV_VER "0.1"

//CONFIG PARSER
typedef struct s_route
{
	std::string location;
	char modifier; //Will support '='
	std::string root_dir;
	std::string upload_root_dir;
	std::vector<std::string> allowed_methods;
	std::string default_dir_file;
	bool dir_listing;
}	t_route;

typedef struct s_conf
{
	std::vector<int> ports;
	std::string host;
	std::vector<std::string> server_names;
	std::vector<std::string> indexs;
	size_t body_limit;
	std::string default_error[N_ERR_IMPLEMENTED];//List of all the default error pages
	std::vector<t_route> routes;//list of all routes
	bool is_default_server;// if true this is the default server to use when no other match HOST header
}	t_conf;

std::vector<t_conf> parseConf(std::string);
int parseRouteFields(char *line, t_route &route);

//NET
typedef struct	s_net
{
	std::list<int> client_fds;
	std::vector<struct sockaddr_in> clients_net;//network information about clients
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

int parse_request(char *request, int fd, t_net&, std::vector<t_conf> servers);
int get_header_id(std::string header_field);
//PARSE REQUEST UTILS
void parse_chunked(size_t i, t_req_line &rl, char *request);
t_conf get_server_conf_for_request(t_req_line &rl, std::vector<t_conf> servers);

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
t_route get_default_route();
t_route get_route_for(t_req_line rl, t_conf conf);
bool method_allowed(std::string method, t_route route);
bool method_supported(std::string method);
void get_dir_listing(std::string dir);
void create_ressource(t_req_line rl, t_route route);
void empty_directory(std::string path);

//DATE
std::string get_imf_fixdate();
std::string get_last_modified(std::string filename);
//CONTENT_TYPE
std::string get_content_type(std::string filename);
//STATUS_CODE
void send_400(t_req_line rl, t_http_res &resp, t_conf conf);
void send_403(t_req_line rl, t_http_res &resp, t_conf conf);
void send_404(t_req_line rl, t_http_res &resp, t_conf conf);
void send_405(t_req_line rl, t_http_res &resp, t_conf conf, t_route route);
void send_413(t_req_line rl, t_http_res &resp, t_conf conf);
void send_501(t_req_line rl, t_http_res &resp, t_conf conf);
void send_505(t_req_line rl, t_http_res &resp, t_conf conf);
void send_200(t_req_line rl, t_http_res &resp, int fd, t_route route);
void send_200_dirlist(t_req_line rl, t_http_res &resp);
void send_200_file_is_a_dir(t_req_line rl, t_http_res &resp, int fd, t_route route);
void send_201_put(t_req_line rl, t_http_res &resp);
void send_204_put(t_req_line rl, t_http_res &resp, t_route route);
void send_204_delete(t_http_res &resp);
//STATUS_CODE UTILS
std::string get_allow_header_for(t_route route);

//UTILS
int print_err(std::string s);
void excerr(std::string msg, int c);
bool file_exists(std::string filename);
bool file_is_dir(std::string filename);
bool is_in_set(char c, char *s);
#endif