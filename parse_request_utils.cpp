#include "webserv.h"

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
	}
	if (!request[i]) // NO EMPTY LINES FOUND ! nginx loops then sends empty resp, we could send 400
		rl.bad_request = true;
	//Could add a check to verify that all chunk sizes were correct
	//if (length != rl.body) -> bad_request;
	rl.headers[CONTENT_LENGTH] = length;
}