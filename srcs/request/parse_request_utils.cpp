#include "../../includes/webserv.h"

void parse_chunked(size_t i, t_req_line &rl, char *request)
{
	char hexset[] = "0123456789abcdef";
	size_t length = 0;
	if (!is_in_set(request[i], hexset) && (rl.bad_request = true))//chunk_size invalid
		return;
	int size = ft_atoi_base(request + i, hexset);
	while (request[i] && request[i] != '\n')//Skip chunk_size ligne and ignore chunk-ext
		i++;
	if (!request[i++] && (rl.bad_request = true))
		return;
	while (size)
	{
		while (request[i] && request[i] != '\n') //read chunk data
		{

			if (request[i] != '\r')
				rl.body += request[i];
			i++;
		}
		if (!request[i++] && (rl.bad_request = true))
			return;
		length += size;
		if (!is_in_set(request[i], hexset) && (rl.bad_request = true))//chunk_size invalid
			return;
		size = ft_atoi_base(request + i, hexset);//GET chunk-size
		while (request[i] && request[i] != '\n')//Skip chunk_size ligne and ignore chunk-ext
			i++;
		if (!request[i++] && (rl.bad_request = true))
			return;
	}
	//PARSE TRAILER FIELD
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
		if (id != -1 && id != TRANSFER_ENCODING && id != CONTENT_LENGTH && id != HOST
			&& id != WWW_AUTHENTICATE && id != CONTENT_TYPE) //ignore forbidden headers as asked by rfc
			rl.headers[id] = header_value;
		if (id == AUTHORIZATION)
		{
			size_t pos = header_value.find(" ");
			rl.auth.type = header_value.substr(0, pos);
			rl.auth.ident = b64decode(header_value.substr(pos + 1));
		}
	}
	if (!request[i]) // NO EMPTY LINES FOUND ! nginx loops then sends empty resp, we could send 400
		rl.bad_request = true;
	//Could add a check to verify that all chunk sizes were correct
	//if (length != rl.body) -> bad_request;
	rl.headers[CONTENT_LENGTH] = std::to_string(length);
}

//ret.first is the hostname and ret.second is the port number
std::pair<std::string, int> parsed_host_header(t_req_line &rl)
{
	std::pair<std::string, int> pair;
	int pos = rl.headers[HOST].find(':');
	int port;
	pair.second = -1;
	if (pos > 0)
	{
		pair.first = rl.headers[HOST].substr(0, pos);
		port = ft_satoi(rl.headers[HOST].substr(pos).c_str());
		if (port < 0)
			rl.bad_request = true;
		else
			pair.second = port;
	}
	else
		pair.first = rl.headers[HOST];

/*	char **sp;
	if (!(sp = ft_split(rl.headers[HOST].c_str(), ':')))
		excerr("Internal error split failed",1);//weird bug where sometimes the next line segfaults
	if (sp[0])
		pair.first = std::string(sp[0]);
	pair.second = -1;
	if (!sp[1])//optional port omitted
	{
		for (size_t i = 0; sp[i]; i++)
			free(sp[i]);
		free(sp);
	return (pair);
	}
	int port = ft_satoi(sp[1]);
	if (port < 0)//Port in header is invalid
		rl.bad_request = true;
	else
		pair.second = port;
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp)*/
	return (pair);
}

t_conf get_server_conf_for_request(t_req_line &rl, std::vector<t_conf> servers, int server_fd)
{
	std::pair<std::string, int> host_pair = parsed_host_header(rl);
	for (std::vector<t_conf>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if ((std::find((*it).server_names.begin(), (*it).server_names.end(), host_pair.first) != (*it).server_names.end() 
			&& host_pair.second == -1 && std::find((*it).fd.begin(), (*it).fd.end(), server_fd) != (*it).fd.end()) //If no port specified in HOST header just compare server_names to the host field value
		|| (std::find((*it).server_names.begin(), (*it).server_names.end(), host_pair.first) != (*it).server_names.end() 
			&& std::find((*it).ports.begin(), (*it).ports.end(), host_pair.second) != (*it).ports.end()// Compare the ports too
			&& std::find((*it).fd.begin(), (*it).fd.end(), server_fd) != (*it).fd.end())) //Check that server_fd handles this serv block
			return (*it);
	}
	for (std::vector<t_conf>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (std::find((*it).fd.begin(), (*it).fd.end(), server_fd) != (*it).fd.end())
			return (*it);
	}
	std::cout << "Shouldn't ever get here" << std::endl; //REMOVE ONCE PROJECT IS FINISHED, might save us ^-^'
	return (servers[0]); // use default server if no other match
}

char **dupEnv(char **envs)
{
	char **ret;
	size_t i = 0;
	while (envs[i])
		i++;
	ret = (char**)malloc(sizeof(char *) * (i+1));
	i = 0;
	while (envs[i])
	{
		ret[i] = ft_strdup(envs[i]);
		i++;
	}
	ret[i] = 0;
	return (ret);
}