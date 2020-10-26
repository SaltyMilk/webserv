#ifndef WEBSERVTEST_H
#define WEBSERVTEST_H

/*
 * libft
 */
#include "../libft/libft.h"

/*
 * network includes
 */
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

/*
 * general includes
 */
#include <string>
#include <string.h>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>

/*
 * header defines
 */
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

/*
 * error defines
 */
#define ERR400 0
#define ERR403 1
#define ERR404 2
#define ERR405 3
#define ERR413 4
#define ERR501 5
#define ERR505 6
#define N_ERR_IMPLEMENTED 7

#define BUFF_SIZE 42

/*
 * version define
 */
#define WEBSERV_VER "0.1"

typedef	struct s_route	t_route;
typedef	struct s_server	t_server;
typedef	struct s_net	t_net;
typedef	struct s_hpf	t_hpf;
typedef	struct s_path	t_path;
typedef	struct s_auth	t_auth;
typedef	struct s_headers t_headers;
typedef	struct s_request t_request;
typedef	struct s_response t_response;

struct	s_route
{
	std::string location;
	char modifier; //Will support '='
	std::string root_dir;
	std::string upload_root_dir;
	std::vector<std::string> allowed_methods;
	std::string default_dir_file;
	bool dir_listing;
	bool cgi;//on or off
	std::string cgi_path;
};

struct	s_server
{
	std::vector<int> ports;
	std::string host;//ip of the server
	std::vector<std::string> server_names;
	std::vector<std::string> indexs;
	size_t body_limit;
	std::string default_error[N_ERR_IMPLEMENTED];//List of all the default error pages
	std::vector<t_route> routes;//list of all routes
	bool is_default_server;// if true this is the default server to use when no other match HOST header
	std::vector<int> fd;//Fd linked to this server
};

struct	s_net
{
	std::vector<struct sockaddr_in> clients_net;//network information about clients
};

struct	s_hpf
{
	std::string host;
	int port;
	int fd;
};

struct	s_path
{
	std::string info;
	std::string script;
	std::string translated;
};

struct	s_auth
{
	std::string	type;
	std::string ident;
};

struct	s_headers
{
	int 		id;
	std::string	value;
};

struct	s_request
{
	std::string method; // GET, PUT, POST,...
	std::string target; // ex: /index, http://https://profile.intra.42.fr/,...
	std::string http_ver;// ex : HTTP/1.1
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
	std::string query;
	t_auth 		auth;
	t_path 		path;
	bool bad_request;//Allows bad_request checks before/while parsing request
	struct sockaddr_in	client_adr;
};

struct	s_response
{
	std::string http_ver;// ex : HTTP/1.1
	std::string status_code; // ex:
	std::string reason_phrase; // GET, PUT, POST,...
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
};

/*
 * Config parsing
 */
std::vector<t_server>	parseConf(std::string);
int		parseRouteFields(char *line, t_route &route);

/*
 * Network
 */
std::string		cinet_ntoa(in_addr_t in);

/*
 * Requests
 */
int		parse_request(char *request, int fd, std::vector<t_server> servers, int server_fd, struct sockaddr_in	client_adr);
int		get_header_id(std::string header_field);
void	parse_chunked(size_t i, t_request &rl, char *request);
t_server get_server_conf_for_request(t_request &rl, std::vector<t_server> servers, int server_fd);
std::pair<std::string, int>	parsed_host_header(t_request &rl);
std::string get_content_type(std::string filename);

/*
 * Response
 */
int answer_request(int client_fd, t_request rl, t_server conf);
//RESPONSE UTILS
int bad_request(t_request rl);
int valid_http_ver(t_request rl);
void handle_absolute_path(t_request &rl);
void parse_query_from_target(t_request &rl);
t_route get_default_route();
t_route get_route_for(t_request rl, t_server conf);
bool method_allowed(std::string method, t_route route);
bool method_supported(std::string method);
void get_dir_listing(std::string dir);
void create_ressource(t_request rl, t_route route);
void empty_directory(std::string path);

/*
 * CGI
 */
char	**get_cgi_envs(t_request &request);
void 	parse_cgi(t_request &request);
std::string execute_cgi(t_request &request, t_route route);

/*
 * Status codes
 */
void send_400(t_request rl, t_response &resp, t_server conf);
void send_403(t_request rl, t_response &resp, t_server conf);
void send_404(t_request rl, t_response &resp, t_server conf);
void send_405(t_request rl, t_response &resp, t_server conf, t_route route);
void send_413(t_request rl, t_response &resp, t_server conf);
void send_501(t_request rl, t_response &resp, t_server conf);
void send_505(t_request rl, t_response &resp, t_server conf);
void send_200(t_request rl, t_response &resp, int fd, t_route route);
void send_200_dirlist(t_request rl, t_response &resp);
void send_200_file_is_a_dir(t_request rl, t_response &resp, int fd, t_route route);
void send_201_put(t_request rl, t_response &resp);
void send_204_put(t_request rl, t_response &resp, t_route route);
void send_204_delete(t_response &resp);
//STATUS_CODE UTILS
std::string get_allow_header_for(t_route route);

/*
 * Utils
 */
int print_err(std::string s);
void excerr(std::string msg, int c);
bool file_exists(std::string filename);
bool file_is_dir(std::string filename);
bool is_in_set(char c, char *s);
void chandler(int sig_num);

#endif WEBSERVTEST_H