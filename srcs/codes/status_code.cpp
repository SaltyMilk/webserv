#include "../../includes/webserv.h"

void send_400(t_req_line rl, t_http_res &resp, t_conf conf)
{	
	resp.headers[CONTENT_TYPE] = ("Content-Type: " + std::string("text/html"));
	resp.status_code = "400";
	resp.reason_phrase = "Bad Request";
	char c;
	int efd = open(conf.default_error[ERR400].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
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
	if (rl.method != "HEAD") // No body for head method
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
	if (rl.method != "HEAD") // No body for head method
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
	if (rl.method != "HEAD") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_405(t_req_line rl, t_http_res &resp, t_conf conf, t_route route)
{
	resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
	resp.status_code = "405";
	resp.reason_phrase = "Method Not Allowed";
	resp.headers[ALLOW] = get_allow_header_for(route);
	char c;
	int efd = open(conf.default_error[ERR405].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}


void send_501(t_req_line rl, t_http_res &resp, t_conf conf)
{
	resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
	resp.status_code = "501";
	resp.reason_phrase = "Not Implemented";
	char c;
	int efd = open(conf.default_error[ERR501].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
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
	if (rl.method != "HEAD") // No body for head method
		while (read(efd, &c, 1) > 0)
			resp.body += c;
	close(efd);
}

void send_200(t_req_line rl, t_http_res &resp, int fd, t_route route)
{
		resp.headers[CONTENT_TYPE] =  "Content-Type: "+ get_content_type(route.root_dir + rl.target); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
		resp.status_code = "200";
		resp.reason_phrase = "OK";
		resp.headers[LAST_MODIFIED] = "Last-Modified: " + get_last_modified(route.root_dir + rl.target);
		char c;
		if (rl.method != "HEAD" && !route.cgi) // No body for head method
			while (read(fd, &c, 1) > 0)
				resp.body += c;
		else if (route.cgi && rl.method != "HEAD")
			resp.body = execute_cgi(rl, route);
	close(fd);
}

void send_200_file_is_a_dir(t_req_line rl, t_http_res &resp, int fd, t_route route)
{
		resp.headers[CONTENT_TYPE] =  "Content-Type: "+ get_content_type(route.default_dir_file); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
		resp.status_code = "200";
		resp.reason_phrase = "OK";
		resp.headers[LAST_MODIFIED] = "Last-Modified: " + get_last_modified(route.default_dir_file);
		char c;
		if (rl.method != "HEAD") // No body for head method
			while (read(fd, &c, 1) > 0)
				resp.body += c;
	close(fd);
}

void send_200_dirlist(t_req_line rl, t_http_res &resp)
{
		int fd = open(".dirlisting.html", O_RDONLY);
		resp.headers[CONTENT_TYPE] =  "Content-Type: " + std::string("text/html");
		resp.status_code = "200";
		resp.reason_phrase = "OK";
		char c;
		if (rl.method != "HEAD") // No body for head method
			while (read(fd, &c, 1) > 0)
				resp.body += c;
	close(fd);
}

void send_201_put(t_req_line rl, t_http_res &resp)
{
	resp.status_code = "201";
	resp.reason_phrase = "Created";
	resp.headers[LOCATION] = "Location: " + rl.target;
}

void send_204_put(t_req_line rl, t_http_res &resp, t_route route)
{
		int fd;
		std::string current_representation;
		fd = open((route.upload_root_dir + rl.target).c_str(), O_RDONLY); //See 201_put comm
		char c;
		while (read(fd, &c, 1) > 0)
				current_representation += c;
		resp.status_code = "204";
		resp.reason_phrase = "No Content";
		if (rl.body == current_representation)
			resp.headers[LAST_MODIFIED] = "Last-Modified: " + get_last_modified(route.root_dir + rl.target);
	close(fd);
}

void send_204_delete(t_http_res &resp)
{
		resp.status_code = "204";
		resp.reason_phrase = "No Content";
}

