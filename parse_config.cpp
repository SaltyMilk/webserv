#include "webserv.h"
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
	else if (std::string(sp[0]) == "default_403")
		conf.default_error[ERR403] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_404")
		conf.default_error[ERR404] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_413")
		conf.default_error[ERR413] = std::string(sp[1]);
	else if (std::string(sp[0]) == "default_505")
		conf.default_error[ERR505] = std::string(sp[1]);
	else
		std::cerr << "Config file warning: cannot set default page for unimplemented error code [" << std::string(sp[0] + 8) << "]" << std::endl;
	if (!file_exists(std::string(sp[1])))
		excerr("Config file error: default error file [" + std::string(sp[1]) + "] couldn't be find.", 1);
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

//Will read inside the location block, ending by reading the final '}' which will then be freed in 
//WIP
/*
void parseRouteConf(t_conf &conf, char *line, int fd)
{
    char *tmp;
    free(line);
    while (get_next_line(fd, &line))
    {
        tmp = ft_strtrim(line, "\t ");
        if (tmp[0] == '}')
            break;
        free(line);
    }
}
*/

void parseRoutes(t_conf &conf, char *line, int fd)
{
    t_route r;
    char **sp = ft_split(line, ' ');
    if (!sp[1])
        excerr("Config file error: empty route field", 1);
    if (sp[2])//USING MODIFIER
    {
        if ((sp[1][0] != '~' && sp[1][0] != '=') || sp[1][1])
            excerr("Config file error: invalid modifier token for location", 1);
        r.modifier = sp[1][0]; 
        r.location = std::string(sp[2]);
        conf.routes.push_back(r);
    }
    else //ex: location /
    {
       r.modifier = 0;
       r.location = std::string(sp[1]);
    }
    for (size_t i = 1; sp[i]; i++)
        free(sp[i]);
    free(sp);
}

void set_default_settings(t_conf &conf)
{
	conf.default_error[ERR400] = "400.html";
	conf.default_error[ERR403] = "403.html";
	conf.default_error[ERR404] = "404.html";
	conf.default_error[ERR413] = "413.html";
	conf.default_error[ERR505] = "505.html";
	
	conf.body_limit = std::string().max_size();
}

t_conf parseConf(std::string filename)
{
	t_conf conf;
	int fd;

	set_default_settings(conf);
	if (!(fd = open(filename.c_str(), O_RDONLY)))
		excerr("Could not open config file. Try with this synax : ./webserv conf_file", 2);
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
        else if (ft_strlen(line) > 8 && std::string(line, 8) == "location")
            parseRoutes(conf, line, fd);
		free(line);
	}
	if (ft_strlen(line) >= 5 && std::string(line, 5) == "port ")
		parsePorts(conf, line);
	else if (ft_strlen(line) >= 6 && std::string(line, 6) == "index ")
		parseIndex(conf, line);	
	else if (ft_strlen(line) >= 10 && std::string(line, 11) ==  "body_limit ")
		parseBodyLimit(conf, line);
	else if (ft_strlen(line) >= 11 && std::string(line, 8) == "default_") //11 = strlen("default_") + 3 for status code
		parseDefaultErrorPage(conf, line);  
    else if (ft_strlen(line) > 8 && std::string(line, 9) == "location ")
        parseRoutes(conf, line, fd);
	free(line);
	if (!conf.ports.size()) //CHECK IF AT LEAST A PORT WAS GIVEN
		excerr("Config file error: missing port number", 1);
	return (conf);
}