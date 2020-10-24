#include "../../includes/webserv.h"

t_headers g_headers[] =
{
		{ACCEPT_CHARSETS, "ACCEPT_CHARSETS"},
		{ACCEPT_LANGUAGE, "ACCEPT_LANGUAGE"},
		{ALLOW, "ALLOW"},
		{AUTHORIZATION, "AUTHORIZATION"},
		{CONTENT_LANGUAGE, "CONTENT_LANGUAGE"},
		{CONTENT_LENGTH, "CONTENT_LENGTH"},
		{CONTENT_LOCATION, "CONTENT_LOCATION"},
		{CONTENT_TYPE, "CONTENT_TYPE"},
		{DATE, "DATE"},
		{HOST, "HOST"},
		{LOCATION, "LOCATION"},
		{REFERER, "REFERER"},
		{RETRY_AFTER, "RETRY_AFTER"},
		{SERVER, "SERVER"},
		{TRANSFER_ENCODING, "TRANSFER_ENCODING"},
		{USER_AGENT, "USER_AGENT"},
		{WWW_AUTHENTICATE, "WWW_AUTHENTICATE"}
};

char	**get_cgi_envs(t_req_line &request, struct sockaddr_in client)
{
	char **envs;
	std::map<std::string, std::string> map;
	std::map<std::string, std::string>::iterator it;
	size_t i = 0;

	//SERVER
	map["GATEWAY_INTERFACE"] = "CGI/1.1";
	map["SERVER_NAME"] = parsed_host_header(request).first;
	map["SERVER_PORT"] = parsed_host_header(request).second;
	map["SERVER_PROTOCOL"] = "HTTP/1.1";
	map["SERVER_SOFTWARE"] = "webserv " + std::string(WEBSERV_VER);
	//REQUEST
	map["QUERY_STRING"] = request.query;
	map["REMOTE_ADDR"] = client.sin_addr.s_addr; //ip du client -- bad
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
		map["CONTENT_LENGTH"] = request.body.length();
		map["CONTENT_TYPE"] = request.headers[CONTENT_TYPE];
	}
    //ici il faut enum jusqu'au bout -- serait mieux avec un for
    while (i < request.headers->length()) {
		if (!request.headers[i].empty())
			map["HTTP_" + g_headers[i].value] = request.headers[i];
	}
    envs = (char **)malloc(sizeof(char *) * (map.size() + 1));
    i = 0;
    for (it = map.begin(); it != map.end(); it++)
    	envs[i++] = ft_strdup((it->first + "=" + it->second).c_str());
    envs[i] = NULL;
    return (envs);
}
