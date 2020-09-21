#include "webserv.h"

int parse_request_line(t_req_line &rl, char** req_lines)
{
	size_t i;
	if (!ft_strncmp(req_lines[0], "GET ", 4))
		rl.method = "GET";
	else if (!ft_strncmp(req_lines[0], "HEAD ", 5))
		rl.method = "HEAD";
	else if (!ft_strncmp(req_lines[0], "POST ", 5))
		rl.method = "POST";
	else if (!ft_strncmp(req_lines[0], "PUT ", 4))
		rl.method = "PUT";
	else if (!ft_strncmp(req_lines[0], "DELETE ", 7))
		rl.method = "DELETE";
	else if (!ft_strncmp(req_lines[0], "CONNECT ", 8))
		rl.method = "CONNECT";
	else if (!ft_strncmp(req_lines[0], "OPTIONS ", 8))
		rl.method = "OPTIONS";
	else if (!ft_strncmp(req_lines[0], "TRACE ", 6))
		rl.method = "TRACE";
	else
		return (1); // NO VALID METHOD FOR REQUEST
	i = ft_strlen(rl.method.c_str()) + 1;
	
	size_t sp_i = i;
	while (req_lines[0][sp_i] != ' ') //Read the target
	{
		if (!req_lines[0][sp_i])
			return (1); // Invalid format no space after target
		sp_i++;	
	}
	rl.target = std::string(req_lines[0]+i, sp_i - i); 
	rl.target[sp_i - i] = 0; // just making sure string is null terminated.
	i = ++sp_i;
	rl.http_ver = req_lines[0]+i;//What's leftover is JUST the version
	return (0);
}


int answer_request(int client_fd, t_req_line rl, t_net &snet)
{
	int fd;
	if (rl.method == "GET")
	{
		if ((fd = open(("."+rl.target).c_str(), O_RDONLY)) == -1)
		{
			std::string s("HTTP/1.1 404 Not Found\r\n\r\n");
			char c;
			int efd = open("404.html", O_RDONLY);
			while (read(efd, &c, 1) > 0)
				s += c;
			s += "\r\n";
			write(client_fd, s.c_str(), ft_strlen(s.c_str()));
		}
		else
		{
			std::string s("HTTP/1.1 200 OK\r\n\r\n");
			char c;
			while (read(fd, &c, 1) > 0)
				s += c;
			s += "\r\n";
			write(client_fd, s.c_str(), ft_strlen(s.c_str()));
		}
		snet.client_fds.remove(client_fd);
		close(client_fd);
	}
	return (0);
}

int	is_field_name(char *req_lines)
{
	if (!ft_strncmp("Accept-Charsets:", req_lines, 16))
		return (1);
	else if (!ft_strncmp("Accept-Language:", req_lines, 16))
		return (1);
	else if (!ft_strncmp("Allow:", req_lines, 6))
		return (1);
	else if (!ft_strncmp("Authorization:", req_lines, 14))
		return (1);
	else if (!ft_strncmp("Content-Language:", req_lines, 17))
		return (1);
	else if (!ft_strncmp("Content-Length:", req_lines, 15))
		return (1);
	else if (!ft_strncmp("Content-Location:", req_lines, 17))
		return (1);
	else if (!ft_strncmp("Content-Type:", req_lines, 13))
		return (1);
	else if (!ft_strncmp("Date:", req_lines, 5))
		return (1);
	else if (!ft_strncmp("Host:", req_lines, 5))
		return (1);
	else if (!ft_strncmp("Last-Modified:", req_lines, 14))
		return (1);
	else if (!ft_strncmp("Location:", req_lines, 9))
		return (1);
	else if (!ft_strncmp("Referer:", req_lines, 8))
		return (1);
	else if (!ft_strncmp("Retry-After:", req_lines, 12))
		return (1);
	else if (!ft_strncmp("Server:", req_lines, 7))
		return (1);
	else if (!ft_strncmp("Transfer-Encoding:", req_lines, 18))
		return (1);
	else if (!ft_strncmp("User-Agent:", req_lines, 11))
		return (1);
	else if (!ft_strncmp("WWW-Authenticate:", req_lines, 17))
		return (1);
	return (0);
}

std::string	field_value(char *req_line)
{
	size_t start, end = ft_strlen(req_line) - 2;

	for (start = 0; req_line[start] != ':'; start++);
	if (req_line[start + 1] == ' ')
		start++;
	for (; req_line[start] == ' '; start++);
	for (; req_line[end] == ' '; end--);
	std::string field_value(req_line);
	return (field_value.substr(start, end));
}

std::string	field_name(char *req_line)
{
	size_t i;

	for (i = 0; req_line[i] != ':'; i++);
	std::string field_name(req_line);
	return (field_name.substr(0, i));
}

int	get_in_map(t_req_line *rl, char **req_lines)
{ 
	for(size_t i = 1; req_lines[i];i++)
		if (is_field_name(req_lines[i]))
			rl->field[field_name(req_lines[i])] = field_value(req_lines[i]);
	return (0);
}

int parse_request(char *request, int fd, t_net &snet)
{
	t_req_line rl;
	char **req_lines; //Will contain each line of the request

	if (!(req_lines = ft_strtok(request, (char *)"\r\n")))
		return (1);
	parse_request_line(rl, req_lines);
	for (size_t i = 0; req_lines[i] ; i++)
		std::cout << req_lines[i] << std::endl;
	get_in_map(&rl, req_lines);
	std::cout << "-----------------------------------------" << std::endl;
	for (std::map<std::string,std::string>::iterator i = rl.field.begin(); i != rl.field.end(); i++)
		std::cout << i->first << "=" << i->second << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	answer_request(fd, rl, snet);	
	return (0);
}