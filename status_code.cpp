#include "webserv.h"

void send_400(t_req_line rl, t_http_res &resp, t_conf conf)
{	
	resp.headers[CONTENT_TYPE] = ("Content-Type: " + std::string("text/html"));
	resp.status_code = "400";
	resp.reason_phrase = "Bad Request";
	char c;
	int efd = open(conf.default_error[ERR400].c_str(), O_RDONLY);
	if (rl.method == "GET") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_403(t_req_line rl, t_http_res &resp, t_conf conf)
{
	resp.headers[CONTENT_TYPE] = ("Content-Type: " + std::string("text/html"));
	resp.status_code = "403";
	resp.reason_phrase = "Forbidden";
	char c;
	int efd = open(conf.default_error[ERR403].c_str(), O_RDONLY);
	if (rl.method == "GET") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_404(t_req_line rl, t_http_res &resp, t_conf conf)
{
	resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
	resp.status_code = "404";
	resp.reason_phrase = "Not Found";
	char c;
	int efd = open(conf.default_error[ERR404].c_str(), O_RDONLY);
	if (rl.method == "GET") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_413(t_req_line rl, t_http_res &resp, t_conf conf)
{
	resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
	resp.status_code = "413";
	resp.reason_phrase = "Payload Too Large";
	char c;
	int efd = open(conf.default_error[ERR413].c_str(), O_RDONLY);
	if (rl.method == "GET") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}


void send_505(t_req_line rl, t_http_res &resp, t_conf conf)
{
	resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
	resp.status_code = "505";
	resp.reason_phrase = "HTTP Version Not Supported";
	char c;
	int efd = open(conf.default_error[ERR505].c_str(), O_RDONLY);
	if (rl.method == "GET") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_200(t_req_line rl, t_http_res &resp, int fd)
{
		resp.headers[CONTENT_TYPE] =  "Content-Type: "+ get_content_type("." + rl.target); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
		resp.status_code = "200";
		resp.reason_phrase = "OK";
		resp.headers[LAST_MODIFIED] = "Last-Modified: " + get_last_modified("." + rl.target);
		char c;
		if (rl.method == "GET") // No body for head method
			while (read(fd, &c, 1) > 0)
				resp.body += c;
	close(fd);
}