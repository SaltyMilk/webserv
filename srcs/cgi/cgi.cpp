#include "../../includes/webserv.h"

void 	parse_cgi(t_req_line &request)
{
	size_t pos = request.target.find(".");
	char 	buff[4097];

	getcwd(buff, 4096);
	while (request.target[pos] != '/' && request.target[pos] != '\0')
		pos++;
	request.path.script = request.target.substr(0, pos);
	request.path.info = request.target.substr(pos + 2);
	request.path.translated = buff + request.path.script;
}

void 	execute_cgi(t_req_line &request, t_conf server, struct sockaddr_in client)
{
	char 	**envs;

	envs = get_cgi_envs(request, server, client);

}

