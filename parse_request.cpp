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

int	is_space_between_field_name_colon(char * req_line)
{
	size_t i = 0;
	
	for(; req_line[i] == ' '; i++);
	for (; req_line[i] != ' ' && req_line[i] != ':'; i++);
	return (req_line[i] != ':');
}

//subject Field Name list
int	is_field_name(char *req_lines)
{
	return (!ft_strncmp("Accept-Charsets:", req_lines, 16) 
	|| !ft_strncmp("Accept-Language:", req_lines, 16)
	|| !ft_strncmp("Allow:", req_lines, 6)
	|| !ft_strncmp("Authorization:", req_lines, 14)
	|| !ft_strncmp("Content-Language:", req_lines, 17)
	|| !ft_strncmp("Content-Length:", req_lines, 15)
	|| !ft_strncmp("Content-Location:", req_lines, 17)
	|| !ft_strncmp("Content-Type:", req_lines, 13)
	|| !ft_strncmp("Date:", req_lines, 5)
	|| !ft_strncmp("Host:", req_lines, 5)
	|| !ft_strncmp("Last-Modified:", req_lines, 14)
	|| !ft_strncmp("Location:", req_lines, 9)
	|| !ft_strncmp("Referer:", req_lines, 8)
	|| !ft_strncmp("Retry-After:", req_lines, 12)
	|| !ft_strncmp("Server:", req_lines, 7)
	|| !ft_strncmp("Transfer-Encoding:", req_lines, 18)
	|| !ft_strncmp("User-Agent:", req_lines, 11)
	|| !ft_strncmp("WWW-Authenticate:", req_lines, 17));
}

//Replace obs-fold by space
int	replace_obs_fold(std::string *field_value)
{
	for (size_t i = 0; field_value[0][i] && field_value[0][i + 1]; i++)
		if (field_value[0][i] == '\r' && field_value[0][i + 1] == '\n')
			field_value[0].replace(i, 2, " ");
	return (0);
}

//Parsing field_value
std::string	field_value(char *req_line)
{
	size_t start, end = ft_strlen(req_line) - 2;//init end -2 "\r\n"
	// 118 -> 120 skip start jusqu'a field_value
	for (start = 0; req_line[start] != ':'; start++);
	if (req_line[start + 1] == ' ')
		start++;
	for (; req_line[start] == ' '; start++);
	// 123 skip end jusqu'a field_value_end
	for (; req_line[end] == ' '; end--);
	//convert to string pour plus de facilité d'usage.
	std::string field_value(req_line);
	//return partie désirée de la string
	replace_obs_fold(&field_value);
	return (field_value.substr(start, end));
}

//Parsing field name
std::string	field_name(char *req_line)
{
	size_t i;
	//get field_name len
	for (i = 0; req_line[i] != ':'; i++);
	//convert to string pour plus de facilité d'usage.
	std::string field_name(req_line);
	//return partie désirée de la string
	return (field_name.substr(0, i));
}

//char** to hashmap
int	get_in_map(t_req_line *rl, char **req_lines)
{ 
	for(size_t i = 1; req_lines[i];i++)
	{
		if (is_space_between_field_name_colon(req_lines[i]))
			std::cout << "space between FN and : -> 400\n" << std::endl;
		if (is_field_name(req_lines[i]))
			rl->headers[field_name(req_lines[i])] = field_value(req_lines[i]);
	}
	return (0);
}



int parse_request(char *request, int fd, t_net &snet, t_conf conf)
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
	for (std::map<std::string,std::string>::iterator i = rl.headers.begin(); i != rl.headers.end(); i++)
		std::cout << i->first << "=" << i->second << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	answer_request(fd, rl, snet, conf);
	return (0);
}