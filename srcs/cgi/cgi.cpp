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

	if (pos == std::string::npos)
		return ;
	getcwd(buff, 4096);
	while (request.target[pos] != '/' && pos < request.target.length())
		pos++;
	request.path.script = request.target.substr(0, pos);
	request.path.translated = buff + request.path.script;
	request.path.info = request.target;
	/*if (pos + 2 < request.target.length())
		request.path.info = request.target.substr(pos + 2);
	else
		request.path.info = request.path.translated;
*/
}

//returns the output of the cgi
std::string execute_cgi(t_req_line &request, t_route route, t_http_res &resp)
{
	char 	**envs;
	char 	**argv = (char**)malloc(3 * sizeof(char *));
	argv[0] = ft_strdup(route.cgi_path.c_str());
	argv[1] = ft_strdup((route.root_dir + request.target).c_str());
	argv[2] = NULL;
	if (!argv[0] && !argv[1])
	{
		free(argv);
		resp.status_code = "500";
		resp.reason_phrase = "Internal Server Error";
		return ("");
	}
	std::string output;
	char buff[1000];
	int fd[2];
	pid_t pid;

	if (!(envs = get_cgi_envs(request)))
	{
		ft_freesplit(argv);
		resp.status_code = "500";
		resp.reason_phrase = "Internal Server Error";
		return ("");
	}
	pipe(fd);
	if ((pid = fork()) < 0)
	{
		close(fd[0]);
		dup2(fd[1], 1); //Everything that would go on stdout goes to fd[1]
		if (request.method == "POST")
		{
			int fd;
			fd = open(".tmpfile", O_WRONLY | O_TRUNC | O_CREAT, 0666);
			ft_putstr_fd(const_cast<char*>(request.body.c_str()), fd);
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
		while ((r = read(fd[0], buff, 1000 -1)) > 0)
		{
			buff[r] = 0;
			output += buff;
		}
		unlink(".tmpfile");
	}
//	std::cout << "DEBUG CGI OUTPUT START:" << std::endl << output << std::endl << "DEBUG CGI OUTPUT END"<< std::endl;
	ft_freesplit(argv);
	ft_freesplit(envs);
	std::cout << "gonna parse status" << std::endl;
	parse_cgi_status(resp, output.c_str());
	std::cout << "finished parsing status" << std::endl;
	return (std::string(output, parse_cgi_headers(resp, output.c_str())));//Return the body of the cgi output without headers and meta infos
}

