#include "webserv.h"

int valid_http_ver(t_req_line rl)
{	//HTTP/1.1
	//[5] is the first '1' this assumes bad request have been parsed first !
	//[6] is checked otherwise HTTP/11.1 would return 1 too 
	return (rl.http_ver[5] == '1' && rl.http_ver[6] == '.');
}
//Note : invalid method doesn't throw bad_request
int bad_request(t_req_line rl)
{
	//CHECK MANDATORY HEADERS
	if (!rl.headers[HOST].length())
		return (1);
	//CHECK TARGET
	if ((!rl.target.length() || !rl.method.length())) //note: if http_version is empty the server just sends nothing
		return (1);
	if (rl.target[0] != '/')
	{
		size_t j = 0;
		while (rl.target[j] && rl.target[j] != ':')
			j++;
		if (rl.target[j] != ':' || j == 0) // ex: mdr/index.html
			return (1);
		if (rl.target[j + 1] != '/' || rl.target[j + 2] != '/')	// ex: mdr:s/ || mdr:/:localhost
			return (1);
		j += 3; // Now looking at http://[this]/index.html
		while (rl.target[j] && rl.target[j] != '/')
			if (rl.target[j++] == '?') //query symbol forbidden in this part
				return (1);
	}
	//CHECK HTTP_VERSION
	if (rl.http_ver.length() && (rl.http_ver.length() < 6  // HTTP_VER will always be at least 8 chars long && if it's empty -> server sends nothing
		|| std::string(rl.http_ver.c_str(), 5) != "HTTP/")) // Check first part [HTTP/]/1.1
		return (1);
	size_t len = rl.http_ver.length();//micro-opti
	size_t i = 5;
	//CHECK SECOND PART HTTP/[1].1
	while (i < len && rl.http_ver[i] != '.') 
		i++;		
	if (!ft_isdigit(rl.http_ver[5]) // HTTP/X -> X is not a digit
		|| rl.http_ver[5] == '0'
		|| (rl.http_ver[i] == '.' && rl.http_ver[5] == '1' && !rl.http_ver[i + 1])//HTTP/1.
		|| (rl.http_ver[5] == '1' && rl.http_ver[6] && rl.http_ver[6] != '.' && !ft_isdigit(rl.http_ver[6])) // ex:HTTP/1a
		|| (rl.http_ver[i] != '.' &&  !rl.http_ver[6] && rl.http_ver[5] == '1'))//EX: HTTP/1 HTTP/0 -> all other send 505, already managed
		return (1);
	// CHECK THIRD PART HTTP/1.[1]
	if (!rl.http_ver[i]) // Done parsing ex: HTTP/12
		return (0);
	size_t j = 1; // starts at 1 to skip the '.'
	while (i + j < len && ft_isdigit(rl.http_ver[i + j]))
		j++;
	if (j > 4 || rl.http_ver[i + j]) // EX(j > 4): HTTP/1.1234 
		return (1);
	return (0);
}

void handle_absolute_path(t_req_line &rl)
{
		if (rl.target[0] != '/')
		{
			size_t i = 0;
			char count = 0;//Will reach 3 max
			while (rl.target[i] && count < 3)
			{
				if (rl.target[i] == '/')
					count++;
				i++;
			}
			if (!rl.target[i--]) 
				rl.target = "/";
			else
				rl.target = std::string(rl.target, i, rl.target.length() - i);
		}
}

void parse_query_from_target(t_req_line &rl)
{
	std::string targ = "";
	size_t i = 0;
	while (rl.target[i] && rl.target[i] != '?')
		targ += rl.target[i++];
	if (rl.target[i] == '?')
		while(rl.target[i])
			rl.query += rl.target[i++];
	rl.target = targ;
}