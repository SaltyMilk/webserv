#include "webserv.h"

t_conf parseConf(std::string filename)
{
	t_conf conf;
	int fd;

	if (!(fd = open(filename.c_str(), O_RDONLY)))
	{
		std::cerr << "Could not open config file. Try with this synax : ./webserv conf_file" << std::endl;
		exit(2);
	}
	char *line;
	while (get_next_line(fd,&line))
	{
		if (std::string(line, 5) == "port ")
		{
			char **sp = ft_split(line, ' ');
			for (size_t i = 1; sp[i]; i++)
			{
				int port;
				if ((port = ft_satoi(sp[i])) == -1) //STRICT ATOI
				{
					std::cerr << "Config file error: invalid port number" << std::endl;
					exit(1);
				}
				else 
					conf.ports.push_back(port); //ADD PORT TO PORT LIST
			}
			for (size_t i = 1; sp[i]; i++)
				free(sp[i]);
			free(sp);
		}
		free(line);
	}
	free(line);
	if (!conf.ports.size()) //CHECK IF AT LEAST A PORT WAS GIVEN
	{
		std::cerr << "Config file error: missing port number" << std::endl;
		exit(1);
	}
	return (conf);

}