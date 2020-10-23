#include "../../includes/webserv.h"

int get_header_id(std::string header_field)
{
	char *lowcase_hf = ft_strlowcase(const_cast<char *>(header_field.c_str()));
	if (!ft_strncmp(lowcase_hf, "accept-charsets", 15))
		return (ACCEPT_CHARSETS);
	if (!ft_strncmp(lowcase_hf, "accept-language", 15))
		return (ACCEPT_LANGUAGE);
	if (!ft_strncmp(lowcase_hf, "allow", 5))
		return (ALLOW);
	if (!ft_strncmp(lowcase_hf, "authorization", 13))
		return (AUTHORIZATION);
	if (!ft_strncmp(lowcase_hf, "content-language", 16))
		return (CONTENT_LANGUAGE);
	if (!ft_strncmp(lowcase_hf, "content-length", 14))
		return (CONTENT_LENGTH);
	if (!ft_strncmp(lowcase_hf, "content-location", 16))
		return (CONTENT_LOCATION);
	if (!ft_strncmp(lowcase_hf, "content-type", 12))
		return (CONTENT_TYPE);
	if (!ft_strncmp(lowcase_hf, "date", 4))
		return (DATE);
	if (!ft_strncmp(lowcase_hf, "host", 4))
		return (HOST);
	if (!ft_strncmp(lowcase_hf, "last-modified", 13))
		return (LAST_MODIFIED);
	if (!ft_strncmp(lowcase_hf, "location", 8))
		return (LOCATION);
	if (!ft_strncmp(lowcase_hf, "referer", 7))
		return (REFERER);
	if (!ft_strncmp(lowcase_hf, "retry-after", 11))
		return (RETRY_AFTER);
	if (!ft_strncmp(lowcase_hf, "server", 6))
		return (SERVER);
	if (!ft_strncmp(lowcase_hf, "transfer-encoding", 17))
		return (TRANSFER_ENCODING);
	if (!ft_strncmp(lowcase_hf, "user-agent", 10))
		return (USER_AGENT);
	if (!ft_strncmp(lowcase_hf, "www-authenticate", 16))
		return (WWW_AUTHENTICATE);
	return (-1);//UNKNOW HEADER -> IGNORE IT
}


void parse_request_line(size_t &i, t_req_line &rl, char *request)
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


void parse_headers(size_t &i, t_req_line &rl, char *request)
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
		if (request[i] == '\n' && request[i] != ':')//Ignore this ex: DATE\r\n
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
		if (id == HOST && rl.headers[HOST].length())//CHECK FOR DUPLICATE HOST HEADER -> Bad request
		{
			rl.bad_request = true;
			return;
		}
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
int parse_request(char *request, int fd, std::vector<t_conf> servers)
{
	t_req_line rl;
	size_t mi = 0; //Master index to parse request
	std::cout <<"errnostart_parse_req"<< strerror(errno) << std::endl;
	parse_request_line(mi, rl, request);
	
	std::cout << "REQUEST LOG:" << std::endl;
	std::cout << rl.method << " " << rl.target << " " << rl.http_ver << std::endl;

	parse_headers(mi, rl, request);
	std::cout<< "host=" << rl.headers[HOST] << std::endl;
	parse_body(mi, rl, request);
	for (int i = 0; i < 18; i++)
		if (rl.headers[i].length())
			std::cout << "header[" <<i << "]" << rl.headers[i] << std::endl;


	std::cout <<"errno_End_parse_req"<< strerror(errno) << std::endl;
	std::cout << rl.body << std::endl;
	std::cout << "END REQUEST LOG" << std::endl;
	answer_request(fd, rl, get_server_conf_for_request(rl, servers));
	return(0);
}
