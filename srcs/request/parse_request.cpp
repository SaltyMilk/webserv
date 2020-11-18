#include "../../includes/webserv.h"

void parse_request_line(size_t &i, t_req_line &rl, const char *request)
{
	rl.bad_request = false;
	//PARSE METHOD
	while (request[i] && request[i] != ' ' && request[i] != '\r' && request[i] != '\n')
		rl.method += request[i++];
	if (request[i] == '\r')
		i++;
	if (!request[i] || (request[i] == '\n' && (i += 1)))//Reached end of request or req_line
		return;
	if (request[i] == ' ') //ignore all SP like nginx (Only one space in RFC)
		i++;
	else
		return;
	//PARSE TARGET
	while (request[i] && request[i] != ' ' && request[i] != '\r' && request[i] != '\n')
		rl.target += request[i++];
	if (request[i] == '\r')
		i++;
	if (!request[i] || (request[i] == '\n' && (i += 1)))//Reached end of request or req_line
		return;
	if (request[i] == ' ') 
		i++;
	else
		return;
	//PARSE HTTP_VER
	while (request[i] && request[i] != ' ' && request[i] != '\r' && request[i] != '\n')
		rl.http_ver += request[i++];
	if (request[i] == '\r')
		i++;
	if ((request[i] == '\n' && (i += 1)))//Reached end of request or req_line
		return;
	else
		rl.bad_request = true;
}

std::list<std::string> parse_content(std::string header_value)
{
	int c = -1;
	char **split = ft_split(header_value.c_str(), ',');
	std::string temp;
	std::list<std::string> content;
	while (split[++c])
	{
		temp = std::string(split[c]).substr(split[c][0] == ' ' ? 1 : 0);
		if (temp.find(';') > 0)
			temp = temp.substr(0, temp.find(';'));
		content.push_back(temp);
	}
	return (content);
}

void parse_headers(size_t &i, t_req_line &rl, const char *request, char **&envp)
{
	while (request[i] && !((request[i] == '\r' && request[i + 1] == '\n') || request[i] == '\n')) // If end of request or end of headers
	{
		std::string header_field = "";
		while (request[i] && request[i] != ':' && request[i] != '\r' && request[i] != '\n' && request[i] != ' ')
			header_field += request[i++];
		if (!request[i] || request[i] == ' ' || (request[i] == '\r' && request[i + 1] != '\n'))//ex: Host[NO \r or \n] or SP between field and : 
		{
			rl.bad_request = true;
			return;
		}
		if (request[i] == '\r')
			i++;
		if (request[i] == '\n')//Ignore this ex: DATE\r\n
		{
			i++;
			continue;
		}
		if (request[i] == ':')
			i++;
		while (request[i] == ' ')//Skip OWS
			i++;
		std::string header_value = "";
		while (request[i])
		{
			if (request[i] == '\r' && request[i + 1] == '\n' &&    //obs-fold found
				 (request[i + 2] == '\t' || request[i + 2] == ' '))    //We will offer back-compatibilty
			{
				header_value += " ";
				i +=3;
			}
			else if (request[i] == '\r' && request[i + 1] == '\n')//End of header value
			{
				i += 2;
				break;
			}
			else if (request[i] == '\n')//For compatibilty with none-conform HTTP REQ
			{
				i++;
				break;
			}
			else if (request[i] == '\r') // \r in the middle of a header value -> 400
			{
				rl.bad_request = true;
				return;
			}
			else
				header_value += request[i++];
		}
		char *trimmed = ft_strtrim(header_value.c_str(), " "); //TRIM extra spaces after header value
		if (!trimmed)
			std::cerr << "ERROR LOG: malloc failed." << std::endl;
		else
		{
			header_value = trimmed;
			free(trimmed);
		}
		if (std::string(ft_strlowcase(const_cast<char *>(header_field.c_str()))) == "content-range" && rl.method == "PUT")//SPECIAL RFC CASE FOR PUT
		{
			rl.bad_request = true;
			return;
		}
		int id = get_header_id(header_field);
		//ADD HEADER AS ENV_VAR FOR CGI
		size_t l = 0;
		while (header_field[l])
		{
			if (header_field[l] == '-')
				header_field[l] = '_';
			l++;
		}
		header_field = ft_strupcase(const_cast<char*>(header_field.c_str()));
		std::string env_header = "HTTP_" + header_field + "=" + header_value;
		if (id == -1) //temp fix might accept all header later
			envp = addEnvVar(envp, ft_strdup(env_header.c_str()));
		//HEADER ADDED AS ENV_VAR FOR CGI
		if (id == HOST && rl.headers[HOST].length())//CHECK FOR DUPLICATE HOST HEADER -> Bad request
		{
			rl.bad_request = true;
			return;
		}
		if (id == AUTHORIZATION)
		{
			size_t pos = header_value.find(' ');
			rl.auth.type = header_value.substr(0, pos);
			rl.auth.ident = b64decode(header_value.substr(pos + 1));
		}
		if (id == ACCEPT_LANGUAGE)
			rl.alanguages = parse_content(header_value);
		if (id == ACCEPT_CHARSETS)
			rl.charsets = parse_content(header_value);
		if (id == CONTENT_LANGUAGE)
			rl.clanguages = parse_content(header_value);
		if (id != -1)
			rl.headers[id] = header_value;
	}
	if (!request[i]) // NO EMPTY LINES FOUND ! nginx loops then sends empty resp, we could send 400
		rl.bad_request = true;
	else if (request[i] == '\n')
		i++; //SET INDEX TO START OF BODY FOR FURTHER PARSING
	else if (request[i] == '\r' && request[i + 1] == '\n')
		i += 2;//SET INDEX TO START OF BODY FOR FURTHER PARSING
}

void parse_body(size_t i, t_req_line &rl, char *request)
{
	if (std::string(ft_strlowcase(const_cast<char *>(rl.headers[TRANSFER_ENCODING].c_str()))) == "chunked")
		parse_chunked(i, rl, request);
	else // Body sent as plain text
	{	
		rl.body = "";
		while (request[i])
			rl.body += request[i++];
	}
}
//Note we accept none-regular http request, meaning every \r\n could be replaced by only \n like on nginx.
t_ans_arg parse_request(char *request, int fd, std::vector<t_conf> servers, int server_fd, struct sockaddr_in client_adr, char **envp)
{
	(void)envp;
	t_ans_arg arg;
	char **serv_env = (char**)malloc(sizeof(char*));
	serv_env[0] = NULL;
	t_req_line rl;
	size_t mi = 0; //Master index to parse request
	rl.bad_request = false;
	rl.client_adr = client_adr;//Get client network infos
	parse_request_line(mi, rl, request);
	
	std::cout << "OLD_REQUEST LOG:" << std::endl;
	std::cout << rl.method << " " << rl.target << " " << rl.http_ver << std::endl;

	parse_headers(mi, rl, request, serv_env);
	//std::cout<< "host=" << rl.headers[HOST] << std::endl;
	parse_body(mi, rl, request);
	for (int i = 0; i < 18; i++)
		if (rl.headers[i].length())
			std::cout << format_header(i, rl.headers[i]) << std::endl;
//	std::cout << rl.body << std::endl;
	std::cout << "END REQUEST LOG" << std::endl;
	//answer_request(fd, rl, get_server_conf_for_request(rl, servers, server_fd), serv_env);
	//Return args for answer
	arg.client_fd = fd;
	arg.rl = rl;
	arg.conf = get_server_conf_for_request(rl, servers, server_fd);
	arg.envp = serv_env;
	return(arg);
}
