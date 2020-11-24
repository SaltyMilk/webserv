#include "../../includes/webserv.h"

void parsePorts(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	if (!sp[1])
			excerr("Config file error: missing argument for port", 1);	
	for (size_t i = 1; sp[i]; i++)
	{
		int port;
		if ((port = ft_satoi(sp[i])) == -1) //STRICT ATOI
			excerr("Config file error: invalid port number", 1);
		else
			conf.ports.push_back(port); //ADD PORT TO PORT LIST
	}
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void parseBodyLimit(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	size_t bl;
	if (!sp[1])
		excerr("Config file error: missing argument for body_limit", 1);
	if ((bl = ft_satost(sp[1])) == (size_t)-1) //STRICT ATOI
		excerr("Config file error: invalid body_limit", 1);
	else
		conf.body_limit = bl; //ADD PORT TO PORT LIST
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void parseIndex(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	for (size_t i = 1; sp[i]; i++)
			conf.indexs.push_back(std::string(sp[i])); //ADD INDEX TO INDEX LIST
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void parseDefaultErrorPage(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	if (!sp[1])
		excerr("Config file error: missing argument for " + std::string(sp[0]), 1);
	if (std::string(sp[0]) == "default_400")
		conf.default_error[ERR400] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_401")
		conf.default_error[ERR401] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_403")
		conf.default_error[ERR403] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_404")
		conf.default_error[ERR404] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_413")
		conf.default_error[ERR413] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_501")
		conf.default_error[ERR501] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_505")
		conf.default_error[ERR505] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_405")
		conf.default_error[ERR405] = std::string(sp[1]);
	else
		std::cerr << "Config file warning: cannot set default page for unimplemented error code [" << std::string(sp[0] + 8) << "]" << std::endl;
	if (!file_exists(std::string(sp[1])))
		excerr("Config file error: default error file [" + std::string(sp[1]) + "] couldn't be find.", 1);
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

//Will read inside the location block, ending by reading the final '}' which will then be freed in 
void parseRouteConf(char *line, int fd, t_route &route)
{
	bool got_in = false;
	get_next_line(fd, &line);
	if (!line || line[0] != '{')
		excerr("Config file error: missing '{' at the start of the ligne after location.", 1);
	if (line[1])
		excerr("Config file error: unexpected token after { in location.", 1);
	free(line);
    while (get_next_line(fd, &line))
    {
    	got_in = true;
		if (!parseRouteFields(line, route))
			return;
    }
	if (got_in && !parseRouteFields(line, route))
		return;
	else
		excerr("Config file error: could not find end of location block, missing or invalid '}' token", 1);
}


void parseRoutes(t_conf &conf, char *line, int fd)
{
    t_route r = get_default_route(); //Initialize with default settings
    char **sp = ft_split(line, ' ');
    if (!sp[1])
        excerr("Config file error: empty route field", 1);
    if (sp[2])//USING MODIFIER
    {
        if (sp[1][0] != '=' || sp[1][1])
            excerr("Config file error: invalid modifier token for location", 1);
        r.modifier = sp[1][0];
       	r.location = (ft_strlen(sp[2]) > 1 && sp[2][ft_strlen(sp[2]) - 1] == '/') ?  std::string(sp[2],0, ft_strlen(sp[2]) - 1) : std::string(sp[2]) ;
		parseRouteConf(line, fd, r);
    }
    else //ex: location /
    {
    	r.modifier = 0;
       	r.location = (ft_strlen(sp[1]) > 1 && sp[1][ft_strlen(sp[1]) - 1] == '/') ?  std::string(sp[1],0, ft_strlen(sp[1]) - 1) : std::string(sp[1]) ;
		parseRouteConf(line, fd, r);
    }
//QUICK PRINT OF PARSED ROUTE FOR DEBUGING
/*	std::cout << "Route debug infos" << std::endl;
	std::cout << "root_dir=" << r.root_dir << "|" << std::endl;
	std::cout << "location=" << r.location << "|" << std::endl;
	std::cout << "modifier=" << r.modifier << "|" << std::endl;
	std::cout << "default_dir_file=" << r.default_dir_file << "|" << std::endl;
	std::cout << "dir_listing=" << r.dir_listing << "|" << std::endl;
	std::cout << "allowed methods=" << std::endl;
	for (std::vector<std::string>::iterator it = r.allowed_methods.begin(); it != r.allowed_methods.end(); it++)
		std::cout << *it << std::endl;
		std::cout << "End of route debug infos" << std::endl;*/
	if (r.cgi && (!r.cgi_path.length() || !r.cgi_exts.size()))
		excerr("Config file error: cgi on but no cgi_path set and/or no cgi_ext.", 1);
    conf.routes.push_back(r);
    for (size_t i = 0; sp[i]; i++)
        free(sp[i]);
    free(sp);
}

void parseServerName(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	if (!sp[1])
			excerr("Config file error: missing argument for server_name", 1);	
	for (size_t i = 1; sp[i]; i++)
		conf.server_names.push_back(std::string(sp[i]));
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void parseHostAddr(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	if (!sp[1])
			excerr("Config file error: missing argument for server_name", 1);	
	conf.host = std::string(sp[1]);
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void set_default_settings(t_conf &conf)
{
	conf.default_error[ERR400] = "www/400.html";
	conf.default_error[ERR401] = "www/401.html";
	conf.default_error[ERR403] = "www/403.html";
	conf.default_error[ERR404] = "www/404.html";
	conf.default_error[ERR405] = "www/405.html";
	conf.default_error[ERR413] = "www/413.html";
	conf.default_error[ERR501] = "www/501.html";
	conf.default_error[ERR505] = "www/505.html";
	
	conf.body_limit = std::string().max_size();
}

t_conf parseServerBlock(int fd)
{
	t_conf conf;

	ft_bzero(&conf, sizeof(conf));
	set_default_settings(conf);

	char *line;
	while (get_next_line(fd, &line))
	{
		if (ft_strlen(line) >= 5 && std::string(line, 5) == "port ")
			parsePorts(conf, line);
		else if (ft_strlen(line) >= 6 && std::string(line, 6) == "index ")
			parseIndex(conf, line);
		else if (ft_strlen(line) >= 10 && std::string(line, 11) ==  "body_limit ")
			parseBodyLimit(conf, line);
		else if (ft_strlen(line) >= 11 && std::string(line, 8) == "default_") //11 = strlen("default_") + 3 for status code
			parseDefaultErrorPage(conf, line);
        else if (ft_strlen(line) > 9 && std::string(line, 9) == "location ")
            parseRoutes(conf, line, fd);
		else if (ft_strlen(line) > 12 && std::string(line, 12) == "server_name ")
			parseServerName(conf, line);
		else if (ft_strlen(line) > 10 && std::string(line, 10) == "host_addr ")
			parseHostAddr(conf, line);
		else if (ft_strlen(line) == 1 && line[0] == '}')
		{
			free(line);
			return (conf);
		}
		free(line);
	}
	if (conf.ports.empty()) //CHECK IF AT LEAST A PORT WAS GIVEN
		excerr("Config file error: missing port number", 1);
	if (ft_strlen(line) == 1 && line[0] == '}')
	{
		free(line);
		return (conf);
	}
	excerr("Config file error: missing '}' at end of server block", 1);
	return (conf);//This will never happend but clang++ is retarded and gives a warning
}

std::vector<t_conf> parseConf(std::string filename)
{
	int fd;
	bool default_serv = true;
	std::vector<t_conf> servers;
	if (file_is_dir(filename))
		excerr("webserv: argument is a directory.", 2);
	if ((fd = open(filename.c_str(), O_RDONLY)) < 0)
		excerr("Config file error: " + std::string(strerror(errno)), 2);
	char *line;
	while (get_next_line(fd, &line))
	{
		if (std::string(line) == "server")
		{
			free(line);
			get_next_line(fd, &line);
			if (!(line[0] == '{' && !line[1]))
				excerr("Config file error: missing { at the beginning of server block (line after 'server')", 2);
			t_conf conf = parseServerBlock(fd);
			conf.is_default_server = default_serv; // First server block will be default
			if (default_serv)
				default_serv = false;
			servers.push_back(conf);
		}
		free(line);
	}
	if (servers.empty())
		excerr("webserv: no server block in config file", 2);
	free(line);
	close(fd);
	return (servers);
}