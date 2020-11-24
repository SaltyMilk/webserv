#include "../../includes/webserv.h"

void parse_cgi_status(t_response &resp, const char *output)
{
	size_t i = 0;
	while (output[i] && output[i] != '\r' && output[i] != '\n')
		i++;
	if (i < 0)
		return;
	char first_line[i + 1];
	ft_bzero(first_line, i + 1);
	size_t j = 0;
	while (j < i)
	{
		first_line[j] = output[j];
		j++; 
	}
	first_line[j] = 0;
	std::cout << "cgi first line =" << first_line << std::endl;
	char **sp;
	sp = ft_split(first_line, ' ');
	if (std::string(sp[0]) == "Status:")
	{
		resp.status_code = std::string(sp[1]);
		resp.reason_phrase = "";
		size_t k = 2;
		while (sp[k])
		{
			if (k > 2)
				resp.reason_phrase += " ";
			resp.reason_phrase += std::string(sp[k++]);
		}
	}
	ft_freesplit(sp);
}

int parse_cgi_headers(t_response &resp, const char *output)
{
	size_t i = 0;
	while (output[i] && !((output[i] == '\r' && output[i + 1] == '\n') || output[i] == '\n')) // If end of output or end of headers
	{
		std::string header_field = "";
		while (output[i] && output[i] != ':' && output[i] != '\r' && output[i] != '\n' && output[i] != ' ')
			header_field += output[i++];
		if (output[i] == '\r')
			i++;
		if (output[i] == '\n')//Ignore this ex: DATE\r\n
		{
			i++;
			continue;
		}
		if (output[i] == ':')
			i++;
		while (output[i] == ' ')//Skip OWS
			i++;
		std::string header_value = "";
		while (output[i])
		{
			if (output[i] == '\r' && output[i + 1] == '\n' &&    //obs-fold found
				 (output[i + 2] == '\t' || output[i + 2] == ' '))    //We will offer back-compatibilty
			{
				header_value += " ";
				i +=3;
			}
			else if (output[i] == '\r' && output[i + 1] == '\n')//End of header value
			{
				i += 2;
				break;
			}
			else if (output[i] == '\n')//For compatibilty with none-conform HTTP REQ
			{
				i++;
				break;
			}
			else
				header_value += output[i++];
		}
		char *trimmed = ft_strtrim(header_value.c_str(), " "); //TRIM extra spaces after header value
		if (!trimmed)
			std::cerr << "ERROR LOG: malloc failed." << std::endl;
		else
		{
			header_value = trimmed;
			free(trimmed);
		}
		int id = get_header_id(header_field);
		if (id != -1)
		{
			/*if (id == CONTENT_TYPE)//THIS IS A TEMP FIX PLEASE USE new function get_header_field after merge !
				resp.headers[id] = "Content-type: " + header_value;*/
			//del lines aboves and uncomment the line under this after merge
			//resp.headers[id] = get_header_field(id) + header_value;
			resp.headers[id] = format_header(id, header_value);
		}
	}
	if (output[i] == '\n')
		i++; //SET INDEX TO START OF BODY FOR FURTHER PARSING
	else if (output[i] == '\r' && output[i + 1] == '\n')
		i += 2;//SET INDEX TO START OF BODY FOR FURTHER PARSING
	return (i);//Return the index at which cgi output body starts
}