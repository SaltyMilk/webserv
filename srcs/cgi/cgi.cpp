#include "../../includes/webserv.h"
//Note for later : we might want to check that cgi_path is an actual file at some point
//Removes all the subdirectories from a path to only return the name of the file
std::string get_file_name(std::string path)
{
	size_t pos;
	if ((pos = path.rfind('/')) == std::string::npos) // No subdirectories do discard
		return (path);
	return (std::string(path, pos + 1));
}

void 	parse_cgi(t_req_line &request)
{
	size_t pos = request.target.find('.');
	char 	buff[4097];

	getcwd(buff, 4096);
	while (request.target[pos] != '/' && pos < request.target.length())
		pos++;
	request.path.script = request.target.substr(0, pos);
	request.path.info = request.target.substr(pos + 2);
	request.path.translated = buff + request.path.script;
}

//returns the output of the cgi
std::string execute_cgi(t_req_line &request, struct sockaddr_in client, t_route route)
{
	char 	**envs;
	char  **argv = (char**)malloc(2*sizeof(char *));
	argv[0] = ft_strdup(get_file_name(request.target).c_str());
	argv[1] = NULL;
	std::string ret;
	char buff[BUFF_SIZE];
	int fd[2];
	pid_t pid;

	envs = get_cgi_envs(request, client);
	pipe(fd);
	pid = fork();
	if (!pid)
	{
		close(fd[0]);
		dup2(fd[1], 1); //Everything that would go on stdout goes to fd[1]
		execve(route.cgi_path.c_str(), argv, envs);
		exit(19);
	}
	else
	{
		int r;
		close(fd[1]);
		while ((r = read(fd[0], buff, BUFF_SIZE - 1)) > 0)
		{
			buff[r] = 0;
			ret += buff;
		}
	}
	std::cout << "DEBUG CGI OUTPUT START:" << std::endl << ret << std::endl << "DEBUG CGI OUTPUT END"<< std::endl;
	free(argv[0]);//free argv
	free(argv);
	for (size_t i = 0; envs[i]; i++)//Free envs
		free(envs[i]);
	free(envs);
	return (ret);
}

