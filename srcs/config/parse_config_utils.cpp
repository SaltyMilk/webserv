#include "../../includes/webserv.h"
int parseRouteFields(char *line, t_route &route)
{
		char *clean_line;
		char **sp;
 
        clean_line = ft_strtrim(line, "\t ");
       	free(line);
		if (clean_line[0] == '}' && clean_line[1])
			excerr("Config file error: unexpected token after } in location.", 1);
        if (clean_line[0] == '}')
		{
			free(clean_line);
            return 0;
		}
		else if (ft_strlen(clean_line) >= 13 && std::string(clean_line, 13) == "allow_method ")
		{
			sp = ft_split(clean_line, ' ');
			if (!sp[1])
				excerr("Config file error: empty allow_method field.", 1);
			for (size_t i = 1; sp[i]; i++)
				route.allowed_methods.push_back(std::string(ft_strupcase(sp[i]))); //add method in CAPITAL LETTERS
			for (size_t i = 0; sp[i]; i++)
				free(sp[i]);
			free(sp);
		}
		else if (ft_strlen(clean_line) >= 9 && std::string(clean_line, 9) == "root_dir ")
		{
			sp = ft_split(clean_line, ' ');
			if (!sp[1])
				excerr("Config file error: empty root_dir field.", 1);
			if (ft_strlen(sp[1]) > 1 && sp[1][ft_strlen(sp[1]) - 1] == '/' ) // Remove final '/' from ex: /dir/ to get /dir
					sp[1][ft_strlen(sp[1]) - 1] = 0;
			route.root_dir = std::string(sp[1]);
			for (size_t i = 0; sp[i]; i++)
				free(sp[i]);
			free(sp);
		}
		else if (ft_strlen(clean_line) >= 12 && std::string(clean_line, 12) == "dir_listing ")
		{
			sp = ft_split(clean_line, ' ');
			if (!sp[1])
				excerr("Config file error: empty dir_listing field.", 1);
			if (std::string(ft_strlowcase(sp[1])) == "off")
				route.dir_listing = false;
			else if (std::string(ft_strlowcase(sp[1])) == "on")
				route.dir_listing = true;
			else
				excerr("Config file error: invalid value for dir_listing.", 1);
			for (size_t i = 0; sp[i]; i++)
				free(sp[i]);
			free(sp);	
		}
		else if (ft_strlen(clean_line) >= 17 && std::string(clean_line, 17) == "default_dir_file ")
		{
			sp = ft_split(clean_line, ' ');
			if (!sp[1])
				excerr("Config file error: empty default_dir_file field.", 1);
			route.default_dir_file = std::string(sp[1]);
			if (!file_exists(route.default_dir_file))
				excerr("Config file error: provided default_dir_file couldn't be find.", 1);
			for (size_t i = 0; sp[i]; i++)
				free(sp[i]);
			free(sp);	
		}
	free(clean_line);
	return 1;
}