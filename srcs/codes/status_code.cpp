#include "../../includes/webserv.h"

void send_500(t_response &resp)
{
	resp.status_code = "500";
	resp.reason_phrase = "Internal Server Error";
	resp.headers[RETRY_AFTER] = "19";
}

void send_400(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "400";
	resp.reason_phrase = "Bad Request";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR400].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1))> 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_401(t_request request, t_response &response, t_server conf, std::string auth_name)
{
	response.status_code = "401";
	response.reason_phrase = "Unauthorized";
	response.headers[WWW_AUTHENTICATE] = format_header(WWW_AUTHENTICATE, "Basic realm=\"" + auth_name + "\"");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR401].c_str(), O_RDONLY);
	if (request.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			response.body += c;
	if (r == -1)
	{
		response.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_403(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "403";
	resp.reason_phrase = "Forbidden";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE,"text/html");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR403].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r==0) 
		close(efd);
}

void send_404(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "404";
	resp.reason_phrase = "Not Found";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR404].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_413(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "413";
	resp.reason_phrase = "Payload Too Large";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR413].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_405(t_request rl, t_response &resp, t_server conf, t_route route)
{
	resp.status_code = "405";
	resp.reason_phrase = "Method Not Allowed";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	resp.headers[ALLOW] = get_allowed_methods(route);
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR405].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_501(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "501";
	resp.reason_phrase = "Not Implemented";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	char c;
	int r= 0;
	int efd = open(conf.default_error[ERR501].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
		resp.body = "";
		close(efd);
	}
	else if (r == 0)
		close(efd);
}

void send_505(t_request rl, t_response &resp, t_server conf)
{
	resp.status_code = "505";
	resp.reason_phrase = "HTTP Version Not Supported";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	char c;
	int r = 0;
	int efd = open(conf.default_error[ERR505].c_str(), O_RDONLY);
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(efd, &c, 1)) > 0)
			resp.body += c;
	if (r == -1)
	{
			resp.body = "";
			close(efd);
	}
	else if (r==0)
		close(efd);
}

void send_200(t_request rl, t_response &resp, int fd, t_route route, char**&envp)
{
	resp.status_code = "200";
	resp.reason_phrase = "OK";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, get_content_type(route.root_dir + rl.target)); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
	resp.headers[LAST_MODIFIED] = format_header(LAST_MODIFIED, get_last_modified(route.root_dir + rl.target));
	char c[BUFF_SIZE];
	int r = 0;
	if ((rl.method != "HEAD" && (!route.cgi || std::find(route.cgi_exts.begin(), route.cgi_exts.end(), get_file_ext(rl.target)) == route.cgi_exts.end()))
		|| (rl.method == "GET" && !rl.query.length())) // No body for head method
	{
		while ((r = GET_FILE_CONTENT(fd, c, BUFF_SIZE - 1)) > 0)
		{	
			c[r] = 0;	
			resp.body += c;
		}
		if (r == -1)
		{
			resp.body = "";
			close(fd);
		}
		else if (r == 0)
			close(fd);
	}
	else if (route.cgi && rl.method != "HEAD")
	{
		resp.body = execute_cgi(rl, route, resp, envp);
		close(fd);
	}
}

void send_200_file_is_a_dir(t_request rl, t_response &resp, int fd, t_route route)
{
	resp.status_code = "200";
	resp.reason_phrase = "OK";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, get_content_type(route.default_dir_file)); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
	resp.headers[LAST_MODIFIED] = format_header(LAST_MODIFIED, get_last_modified(route.default_dir_file));
	char c[BUFF_SIZE];
	int r = 0;
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(fd, c, BUFF_SIZE - 1)) > 0)
		{
			c[r] = 0;
			resp.body += c;
		}
	if (r == -1)
	{
		resp.body = "";
		close(fd);
	}
	else if (r == 0)
		close(fd);
}

void send_200_dirlist(t_request rl, t_response &resp)
{
	resp.status_code = "200";
	resp.reason_phrase = "OK";
	resp.headers[CONTENT_TYPE] = format_header(CONTENT_TYPE, "text/html");
	int fd = open(".dirlisting.html", O_RDONLY);
	char c[BUFF_SIZE];
	int r = 0;
	if (rl.method != "HEAD") // No body for head method
		while ((r = GET_FILE_CONTENT(fd, c, BUFF_SIZE - 1)) > 0)
		{
			c[r] = 0;
			resp.body += c;
		}
	if (r == -1)
	{
		resp.body = "";
		close(fd);
	}
	else if (r == 0)
		close(fd);
}

void send_201_put(t_request rl, t_response &resp)
{
	resp.status_code = "201";
	resp.reason_phrase = "Created";
	resp.headers[LOCATION] = format_header(LOCATION, rl.target);
}

void send_204_put(t_request rl, t_response &resp, t_route route)
{
	int fd;
	std::string current_representation;
	fd = open((route.upload_root_dir + rl.target).c_str(), O_RDONLY); //See 201_put comm
	char c[BUFF_SIZE];
	int r = 0;
	while ((r = GET_FILE_CONTENT(fd, c, BUFF_SIZE)) > 0)
	{
		c[r] = 0;
		current_representation += c;
	}
	if (r == -1)
	{
		current_representation = "";
		close(fd);
	}
	else if (r == 0)
	{	
		resp.status_code = "204";
		resp.reason_phrase = "No Content";
		if (rl.body == current_representation)
			resp.headers[LAST_MODIFIED] = format_header(LAST_MODIFIED, get_last_modified(route.root_dir + rl.target));
		close(fd);
	}
	resp.status_code = "204";
	resp.reason_phrase = "No Content";
	if (rl.body == current_representation)
			resp.headers[LAST_MODIFIED] = format_header(LAST_MODIFIED, get_last_modified(route.root_dir + rl.target));
}

void send_204_delete(t_response &resp)
{
	resp.status_code = "204";
	resp.reason_phrase = "No Content";
}

