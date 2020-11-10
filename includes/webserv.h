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
#include <signal.h>
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

#ifndef BUFF_SIZE
	#define BUFF_SIZE 42
#endif


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
#define N_ERR_IMPLEMENTED 9
//Defines to be used with conf::default_error
#define ERR400 0
#define ERR401 1
#define ERR403 2
#define ERR404 3
#define ERR405 4
#define ERR413 5
#define ERR500 6
#define ERR501 7
#define ERR505 8

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
	bool cgi;//on or off 
	std::string cgi_path;
	std::vector<std::string> cgi_exts;
	std::string auth_name;
	std::string auth_user;
	bool auth;
}	t_route;

typedef struct s_conf
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
}	t_conf;

std::vector<t_conf> parseConf(std::string);
int parseRouteFields(char *line, t_route &route);

//NET
typedef struct	s_net
{
	std::vector<struct sockaddr_in> clients_net;//network information about clients
}				t_net;

typedef struct	host_port_fd //Gives us the server socket corresponding to a port and host
{
	std::string host;
	int port;
	int fd;
}				 t_hpf;
std::string cinet_ntoa(in_addr_t in);

typedef struct	s_path t_path;
struct s_path
{
	std::string info;
	std::string script;
	std::string translated;
};

typedef struct	s_auth t_auth;
struct s_auth
{
	std::string	type;
	std::string ident;
};

typedef struct	s_headers t_headers;
struct s_headers
{
	int 		id;
	std::string	value;
	std::string name;
};


//PARSER
typedef struct	s_request_line
{
	std::string method; // GET, PUT, POST,...
	std::string target; // ex: /index, http://https://profile.intra.42.fr/,...
	std::string http_ver;// ex : HTTP/1.1
	std::string headers[18]; //headers are indexed like in project's subject
	std::list<std::string> alanguages;
	std::list<std::string> clanguages;
	std::list<std::string> charsets;
	std::string body;
	std::string query;
	t_auth 		auth;
	t_path 		path;
	bool bad_request;//Allows bad_request checks before/while parsing request
	struct sockaddr_in	client_adr;
}				t_req_line;

extern std::vector<int> 	serv_socket;

int parse_request(char *request, int fd, std::vector<t_conf> servers, int server_fd, struct sockaddr_in	client_adr);
int get_header_id(std::string header_field);
//PARSE REQUEST UTILS
void parse_chunked(size_t i, t_req_line &rl, char *request);
t_conf get_server_conf_for_request(t_req_line &rl, std::vector<t_conf> servers, int server_fd);
std::pair<std::string, int> parsed_host_header(t_req_line &rl);
std::string cinet_ntoa(in_addr_t in);

//RESPONSE
typedef struct	s_http_res
{
	std::string http_ver;// ex : HTTP/1.1
	std::string status_code; // ex: 
	std::string reason_phrase; // GET, PUT, POST,...
	std::string headers[18]; //headers are indexed like in project's subject
	std::string body;
}				t_http_res;
int answer_request(int client_fd, t_req_line rl, t_conf conf);
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

//CGI
char	**get_cgi_envs(t_req_line &request);
void 	parse_cgi(t_req_line &request);
std::string	format_header(int header, std::string value);
std::string get_header_field(int header);
std::string execute_cgi(t_req_line &request, t_route route, t_http_res &resp);
int parse_cgi_headers(t_http_res &resp, const char *output);
void parse_cgi_status(t_http_res &resp, const char *output);

//DATE
std::string get_imf_fixdate();
std::string get_last_modified(std::string filename);
//CONTENT_TYPE
std::string get_content_type(std::string filename);
//STATUS_CODE
void send_400(t_req_line rl, t_http_res &resp, t_conf conf);
void send_401(t_req_line request, t_http_res &response, t_conf conf, std::string auth_name);
void send_403(t_req_line rl, t_http_res &resp, t_conf conf);
void send_404(t_req_line rl, t_http_res &resp, t_conf conf);
void send_405(t_req_line rl, t_http_res &resp, t_conf conf, t_route route);
void send_413(t_req_line rl, t_http_res &resp, t_conf conf);
void send_418(t_http_res &resp);
void send_500(t_req_line &rl, t_http_res &resp, t_conf conf);
void send_501(t_req_line rl, t_http_res &resp, t_conf conf);
void send_505(t_req_line rl, t_http_res &resp, t_conf conf);
void send_200(t_req_line rl, t_http_res &resp, int fd, t_route route);
void send_200_dirlist(t_req_line rl, t_http_res &resp);
void send_200_file_is_a_dir(t_req_line rl, t_http_res &resp, int fd, t_route route);
void send_201_put(t_req_line rl, t_http_res &resp);
void send_204_put(t_req_line rl, t_http_res &resp, t_route route);
void send_204_delete(t_http_res &resp);
//STATUS_CODE UTILS
std::string get_allowed_methods(t_route route);

//UTILS
int print_err(std::string s);
void excerr(std::string msg, int c);
bool file_exists(std::string filename);
bool file_is_dir(std::string filename);
bool is_in_set(char c, char *s);
std::string get_file_ext(std::string file);
void chandler(int sig_num);
std::string b64decode(const std::string& str64);
std::string str_replace(std::string str, const std::string &old_key, const std::string &new_key);
void debug(std::string name, std::string content);
#endif