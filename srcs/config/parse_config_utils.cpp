#include "../../includes/webserv.h"
void parseBodyLimit(t_route &route, char *line)
{
	char **sp = ft_split(line, ' ');
	size_t bl;
	if (!sp[1])
		excerr("Config file error: missing argument for body_limit", 1);
	if ((bl = ft_satost(sp[1])) == (size_t)-1) //STRICT ATOI
		excerr("Config file error: invalid body_limit", 1);
	else
		route.body_limit = bl; //ADD PORT TO PORT LIST
	for (size_t i = 0; sp[i]; i++)
		free(sp[i]);
	free(sp);
}
int parseRouteFields(char *line, t_route &route)
{
	char *clean_line;
	char **sp;
	std::string	temp;
 
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
		route.allowed_methods.erase(route.allowed_methods.begin(), route.allowed_methods.end());
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty allow_method field.", 1);
		for (size_t i = 1; sp[i]; i++)
			route.allowed_methods.push_back(std::string(ft_strupcase(sp[i]))); //add method in CAPITAL LETTERS
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 9 && std::string(clean_line, 9) == "root_dir ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty root_dir field.", 1);
		if (ft_strlen(sp[1]) > 1 && sp[1][ft_strlen(sp[1]) - 1] == '/' ) // Remove final '/' from ex: /dir/ to get /dir
			sp[1][ft_strlen(sp[1]) - 1] = 0;
		route.root_dir = std::string(sp[1]);
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 16 && std::string(clean_line, 16) == "upload_root_dir ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty upload_root_dir field.", 1);
		if (ft_strlen(sp[1]) > 1 && sp[1][ft_strlen(sp[1]) - 1] == '/' ) // Remove final '/' from ex: /dir/ to get /dir
			sp[1][ft_strlen(sp[1]) - 1] = 0;
		route.upload_root_dir = std::string(sp[1]);
		ft_freesplit(sp);
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
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 17 && std::string(clean_line, 17) == "default_dir_file ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty default_dir_file field.", 1);
		route.default_dir_file = std::string(sp[1]);
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 9 && std::string(clean_line, 9) == "cgi_path ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty cgi_path field.", 1);
		route.cgi_path = std::string(sp[1]);
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 4 && std::string(clean_line, 4) == "cgi ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty cgi field.", 1);
		if (std::string(ft_strlowcase(sp[1])) == "off")
			route.cgi = false;
		else if (std::string(ft_strlowcase(sp[1])) == "on")
			route.cgi = true;
		else
			excerr("Config file error: invalid value for cgi.", 1);
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 8 && std::string(clean_line, 8) == "cgi_ext ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: empty cgi_ext field.", 1);
		for (size_t i = 1; sp[i]; i++)
			route.cgi_exts.push_back(std::string(sp[i]));
		for (size_t i = 0; sp[i]; i++)
			free(sp[i]);
		free(sp);
	}
	else if (ft_strlen(clean_line) >= 11 && std::string(clean_line, 11) == "auth_basic ")
	{
		sp = ft_splitquotes(clean_line);
		if (!sp[1])
			excerr("Config file error: empty auth_basic field.", 1);
		temp = std::string(sp[1]);
		if (temp[0] == '"')
			route.auth_name = temp.substr(1, temp.length() - 1);
		else
			route.auth_name = temp;
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 10 && std::string(clean_line, 10) == "auth_user ")
	{
		sp = ft_split(clean_line, ' ');
		if (!sp[1])
			excerr("Config file error: no auth_user set.", 1);
		route.auth_user = std::string(sp[1]);
		ft_freesplit(sp);
	}
	else if (ft_strlen(clean_line) >= 11 && std::string(clean_line, 11) == "body_limit ")
		parseBodyLimit(route, clean_line);
	if (!route.auth_name.empty() && !route.auth_user.empty())
		route.auth = true;
	free(clean_line);
	return (1);
}