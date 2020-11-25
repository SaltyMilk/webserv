#include "../../includes/webserv.h"
//Note for later : we might want to check that cgi_path is an actual file at some point
//Removes all the subdirectories from a path to only return the name of the file

void 	parse_cgi(t_request &request)
{
	size_t pos = request.target.find('.');
	char 	buff[4097];

	if (pos == std::string::npos)
		return ;
	getcwd(buff, 4096);
	while (request.target[pos] != '/' && pos < request.target.length())
		pos++;
	request.path.script = request.target.substr(0, pos);
	request.path.translated = buff + request.path.script;
	request.path.info = request.target;
}

//returns the output of the cgi
std::string execute_cgi(t_request &request, t_route route, t_response &resp, char **&envp)
{
	std::string output;
	char buff[BUFF_SIZE];
	int fd[2];
	pid_t pid;
	char 	**envs;
	char	**argv;

	if (!(argv = (char **)ft_memalloc(3 * sizeof(char *))))
	{
		request.err500 = true;
		resp.err500 = true;
		return ("");
	}
	argv[0] = ft_strdup(route.cgi_path.c_str());
	argv[1] = ft_strdup((route.root_dir + request.target).c_str());
	argv[2] = NULL;
	if (!argv[0] || !argv[1] || !(envs = get_cgi_envs(request, envp)))
	{
		free(argv);
		request.err500 = true;
		resp.err500 = true;
		return ("");
	}

	pipe(fd);
	pid = fork();
	if (!pid)
	{
		close(fd[0]);
		dup2(fd[1], 1); //Everything that would go on stdout goes to fd[1]
		if (request.method == "POST")
		{
			int fd;
			int r;
			fd = open(".tmpfile", O_WRONLY | O_TRUNC | O_CREAT, 0666);
			r = PUT_FILE(fd, const_cast<char*>(request.body.c_str()), request.body.length());
			if (r == -1 ||  r == 0)
			{
				std::cout << "Warning: cgi received bad info, but server will continue procedures" << std::endl;
			}
			close(fd);
			fd = open(".tmpfile", O_RDONLY);
			dup2(fd, 0);
		}
		execve(route.cgi_path.c_str(), argv, envs);
		exit(0);
	}
	else
	{
		int r;
		close(fd[1]);
		while ((r = GET_FILE_CONTENT(fd[0], buff, BUFF_SIZE -1)) > 0)
		{
			buff[r]=0;
			output += buff;
		}
		if (r == 0)
			std::cout << "Read cgi output correctly, nice." << std::endl;
		else if (r == -1)
			std::cout << "Warning bad cgi output, no problem server is still operational for this request" << std::endl;
	}
	close(fd[1]);
	close(fd[0]);
//	std::cout << "DEBUG CGI OUTPUT START:" << std::endl << output << std::endl << "DEBUG CGI OUTPUT END"<< std::endl;
	for (size_t i = 0; argv[i]; i++)
		free(argv[i]);
	free(argv);
	//for (size_t i = 0; envs && envs[i]; i++)
	//	free(envs[i]);
	//free(envs);
	parse_cgi_status(resp, output.c_str());
/*	size_t k = 0;
	std::cout <<  "DEBUG BEGINING OF CGI OUTPUT" << std::endl;
	while (output[k] && !(output[k] == '\r' && output[k + 1] == '\n' && output[k+2] == '\r' && output[k +3] == '\n'))
		PUT_FILE(1, output.c_str() + k++, 1);
	std::cout << std::endl <<  "DEBUG END OF CGI OUTPUT" << std::endl;
	*/return (std::string(output, parse_cgi_headers(resp, output.c_str())));//Return the body of the cgi output without headers and meta infos
}

