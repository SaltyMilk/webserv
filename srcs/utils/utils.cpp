#include "../../includes/webserv.h"

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

	if (i == std::string::npos)
		return (std::string(""));
	while (i > 0 && file[i] != '.' && file[i] != '/')
		i--;
	if (i <= 0 || file[i] == '/')
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

char **addEnvVar(char **envs, char *var)
{
	char **ret;
	size_t i = 0;
	if (!envs)
	{
		if (!(ret = (char **)ft_memalloc(sizeof(char *) * 2)))
			return (NULL);
		ret[0] = var;
		ret[1] = NULL;
		return (ret);
	}
	while (envs[i])
		i++;
	if (!(ret = (char**)ft_memalloc(sizeof(char *) * (i+2))))
	{
		for (size_t k = 0; envs && envs[k]; k++)
			free(envs[k]);
		free(envs);
		free(var);
		return (NULL);
	}
	i= 0;
	while (envs[i])
	{
		if (!(ret[i] = ft_strdup(envs[i])))
		{
			for (size_t k = 0; ret && ret[k]; k++)
				free(ret[k]);
			free(ret);
			for (size_t k = 0; envs && envs[k]; k++)
				free(envs[k]);
			free(envs);
			free(var);
			return NULL;
		}
		i++;
	}
	ret[i] = var;
	ret[i + 1] = NULL;
	for (size_t k = 0; envs && envs[k]; k++)
		free(envs[k]);
	free(envs);
	free(var);
	return (ret);
}

/*
** Returns seconds since epoch
*/

time_t	get_time_sec(void)
{
	struct timeval ct;

	gettimeofday(&ct, 0);
	return (ct.tv_sec);
}
