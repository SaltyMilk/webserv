#include "../../includes/webserv.h"

char	**get_cgi_envs(t_request &request, char**&envp)
{
	//char **envs;
	std::map<std::string, std::string> map;
	std::map<std::string, std::string>::iterator it;
	//size_t i = 0;
	//SERVER
	map["GATEWAY_INTERFACE"] = "CGI/1.1";
	map["SERVER_NAME"] = parsed_host_header(request).first;
	map["SERVER_PORT"] = std::to_string(parsed_host_header(request).second);
	map["SERVER_PROTOCOL"] = "HTTP/1.1";
	map["SERVER_SOFTWARE"] = "webserv " + std::string(WEBSERV_VER);
	//REQUEST
	map["QUERY_STRING"] = request.query;
	map["REMOTE_ADDR"] = cinet_ntoa(request.client_adr.sin_addr.s_addr);
	map["REQUEST_METHOD"] = request.method;
	map["REQUEST_URI"] = request.target;
	//AUTH
	if (!request.headers[AUTHORIZATION].empty())
	{
		map["AUTH_TYPE"] = request.auth.type;
		map["REMOTE_IDENT"] = request.auth.ident;
		map["REMOTE_USER"] = request.auth.ident;
	}
    //SCRIPT
	map["PATH_INFO"] = request.path.info;
	map["PATH_TRANSLATED"] = request.path.translated;
	map["SCRIPT_NAME"] = request.path.script;
    //CONTENT
    if (request.method == "POST" || request.method == "PUT")
	{
		map["CONTENT_LENGTH"] = std::to_string(request.body.length());
		map["CONTENT_TYPE"] = request.headers[CONTENT_TYPE];
	}
    //ici il faut enum jusqu'au bout -- serait mieux avec un for
 /*   while (i < request.headers->length()) {
		if (!request.headers[i].empty())
			map["HTTP_" + get_header_field(i)] = request.headers[i];
	}*/
   // envs = (char **)malloc(sizeof(char *) * (map.size() + 1));
    for (it = map.begin(); it != map.end(); it++) 
 	  	envp = addEnvVar(envp, ft_strdup((it->first + "=" + it->second).c_str()));
  	//  	envs[i++] = ft_strdup((it->first + "=" + it->second).c_str());
/*	std::cout << "START" << std::endl;
	for (size_t i = 0; envp[i]; i++)
		std::cout << envp[i] << std::endl;
	std::cout << "END" << std::endl;*/
    return (envp);
}
