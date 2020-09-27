#include "webserv.h"
void parsePorts(t_conf &conf, char *line)
{
			char **sp = ft_split(line, ' ');
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

t_conf parseConf(std::string filename)
{
	t_conf conf;
	int fd;

	if (!(fd = open(filename.c_str(), O_RDONLY)))
		excerr("Could not open config file. Try with this synax : ./webserv conf_file", 2);
	char *line;
	while (get_next_line(fd,&line))
	{
		if (std::string(line, 5) == "port ")
			parsePorts(conf, line);

		free(line);
	}
	free(line);
	if (!conf.ports.size()) //CHECK IF AT LEAST A PORT WAS GIVEN
		excerr("Config file error: missing port number", 1);
	return (conf);

}