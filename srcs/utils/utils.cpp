#include "../../includes/webserv.h"
int print_err(std::string s)
{
	std::cout << s;
	return (1);
}
//Cerr msg and exits with code c
void excerr(std::string msg, int c)
{
	std::cerr << msg << std::endl;
	exit(c);
}
//works with file's path too
std::string get_file_ext(std::string file)
{
	size_t i = file.length() - 1;
	while (i > 0 && file[i] != '.' && file[i] != '/')
		i--;
	if (file[i] == '/' || !i)
		return (std::string(""));//No extension for file
	return (std::string(file, i));
}

bool file_exists(std::string filename)
{
	struct stat buff;
	return(!stat(filename.c_str(), &buff));
}

bool file_is_dir(std::string filename)
{
	struct stat buff;
	ft_bzero(&buff, sizeof(buff));
	stat(filename.c_str(), &buff);
	return (buff.st_mode & S_IFDIR);
}

bool is_in_set(char c, char *s)
{
	while (*s)
	{
		if (*s == c)
			return(true);
		s++;
	}
	return (false);
}
//Return a string with first occurence of old_key replaced by new_key substituted in str
//MAIN USE IS FOR TARGET PARSING WITH LOCATIONS USE WITH CAUTION
std::string str_replace(std::string str, const std::string &old_key, const std::string &new_key)
{
	size_t start_pos = str.find(old_key);
	if(start_pos == std::string::npos)
		return str;
	str.replace(start_pos, old_key.length(), new_key);
	return str;
}

void debug(std::string name, std::string content)
{
	std::cout << name << content << std::endl;
}
