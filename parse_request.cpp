#include "parser.h"

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
	while (req_lines[0][sp_i] != ' ')
	{
		if (!req_lines[0][sp_i])
			return (1); // Invalid format no space after target
		sp_i++;	
	}
	rl.target = std::string(req_lines[0]+i, sp_i - i);
	i = ++sp_i;
	rl.http_ver = req_lines[0]+i;
	return (0);
}


int answer_request(int client_fd, t_req_line rl)
{
	int fd;
	if (rl.method == "GET")
	{
		if ((fd = open(("."+rl.target).c_str(), O_RDONLY)) == -1)
		{
			std::string s("HTTP/1.1 404 Not Found\r\nConnection: Closed\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n");
			char c;
			int efd = open("404.html", O_RDONLY);
			while (read(efd, &c, 1) > 0)
				s += c;
			s += "\r\n";
			write(client_fd, s.c_str(), ft_strlen(s.c_str()));
			std::cout << s << "----- " << std::endl;
		}
		else
		{
			std::string s("HTTP/1.1 200 OK\r\n\r\n");
			char c;
			while (read(fd, &c, 1) > 0)
				s += c;
			s += "\r\n";
			std::cout << s << "-------" << std::endl;
			write(client_fd, s.c_str(), ft_strlen(s.c_str()));
		}
	}
	return (0);
}

int parse_request(char *request, int fd)
{
	t_req_line rl;

	char **req_lines; //Will contain each line of the request
	if (!(req_lines = ft_strtok(request, (char *)"\r\n")))
		return (1);
	parse_request_line(rl, req_lines);
	std::cout << "parsed infos" << std::endl;
	for (size_t i = 0; req_lines[i]; i++)
		std::cout << req_lines[i] << std::endl;
	std::cout << "--------" << std::endl;
	std::cout << "method=" << rl.method << std::endl;
	std::cout << "target=" << rl.target << std::endl;
	std::cout << "http_ver=" << rl.http_ver << std::endl;
	answer_request(fd, rl);	
	return (0);
}