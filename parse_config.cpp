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
	for (size_t i = 1; sp[i]; i++)
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
	for (size_t i = 1; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

void parseIndex(t_conf &conf, char *line)
{
	char **sp = ft_split(line, ' ');
	for (size_t i = 1; sp[i]; i++)
			conf.indexs.push_back(std::string(sp[i])); //ADD INDEX TO INDEX LIST
	for (size_t i = 1; sp[i]; i++)
		free(sp[i]);
	free(sp);
}

t_conf parseConf(std::string filename)
{
	t_conf conf;
	int fd;

	conf.body_limit = filename.max_size(); // filename has no use here just want to call non static function
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
		free(line);
	}
	if (ft_strlen(line) >= 5 && std::string(line, 5) == "port ")
		parsePorts(conf, line);
	else if (ft_strlen(line) >= 6 && std::string(line, 6) == "index ")
		parseIndex(conf, line);	
	else if (ft_strlen(line) >= 10 && std::string(line, 11) ==  "body_limit ")
		parseBodyLimit(conf, line);
	free(line);
	if (!conf.ports.size()) //CHECK IF AT LEAST A PORT WAS GIVEN
		excerr("Config file error: missing port number", 1);
	return (conf);
}