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