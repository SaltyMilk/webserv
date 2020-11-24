#include "../../includes/webserv.h"

int valid_http_ver(t_request rl)
{	//HTTP/1.1
	//[5] is the first '1' this assumes bad request have been parsed first !
	//[6] is checked otherwise HTTP/11.1 would return 1 too 
	return (rl.http_ver[5] == '1' && rl.http_ver[6] == '.');
}
//Note : invalid method doesn't throw bad_request
int bad_request(t_request rl)
{
	//CHECK MANDATORY HEADERS
	if (!rl.headers[HOST].length())
		return (1);
	//CHECK TARGET
	if ((!rl.target.length() || !rl.method.length())) //note: if http_version is empty the server just sends nothing
		return (1);
	if (rl.target[0] != '/')
	{
		size_t j = 0;
		while (rl.target[j] && rl.target[j] != ':')
			j++;
		if (rl.target[j] != ':' || j == 0) // ex: mdr/index.html
			return (1);
		if (rl.target[j + 1] != '/' || rl.target[j + 2] != '/')	// ex: mdr:s/ || mdr:/:localhost
			return (1);
		j += 3; // Now looking at http://[this]/index.html
		while (rl.target[j] && rl.target[j] != '/')
			if (rl.target[j++] == '?') //query symbol forbidden in this part
				return (1);
	}
	//CHECK HTTP_VERSION
	if (rl.http_ver.length() && (rl.http_ver.length() < 6  // HTTP_VER will always be at least 8 chars long && if it's empty -> server sends nothing
				|| std::string(rl.http_ver.c_str(), 5) != "HTTP/")) // Check first part [HTTP/]/1.1
		return (1);
	size_t len = rl.http_ver.length();//micro-opti
	size_t i = 5;
	//CHECK SECOND PART HTTP/[1].1
	while (i < len && rl.http_ver[i] != '.') 
		i++;		
	if (!ft_isdigit(rl.http_ver[5]) // HTTP/X -> X is not a digit
			|| rl.http_ver[5] == '0'
			|| (rl.http_ver[i] == '.' && rl.http_ver[5] == '1' && !rl.http_ver[i + 1])//HTTP/1.
			|| (rl.http_ver[5] == '1' && rl.http_ver[6] && rl.http_ver[6] != '.' && !ft_isdigit(rl.http_ver[6])) // ex:HTTP/1a
			|| (rl.http_ver[i] != '.' &&  !rl.http_ver[6] && rl.http_ver[5] == '1'))//EX: HTTP/1 HTTP/0 -> all other send 505, already managed
		return (1);
	// CHECK THIRD PART HTTP/1.[1]
	if (!rl.http_ver[i]) // Done parsing ex: HTTP/12
		return (0);
	size_t j = 1; // starts at 1 to skip the '.'
	while (i + j < len && ft_isdigit(rl.http_ver[i + j]))
		j++;
	if (j > 4 || rl.http_ver[i + j]) // EX(j > 4): HTTP/1.1234 
		return (1);
	return (0);
}

void handle_absolute_path(t_request &rl)
{
	if (rl.target[0] != '/')
	{
		size_t i = 0;
		char count = 0;//Will reach 3 max
		while (rl.target[i] && count < 3)
		{
			if (rl.target[i] == '/')
				count++;
			i++;
		}
		if (!rl.target[i--])
			rl.target = "/";
		else
			rl.target = std::string(rl.target, i, rl.target.length() - i);
	}
}

void parse_query_from_target(t_request &rl)
{
	std::string targ = "";
	size_t i = 0;
	while (rl.target[i] && rl.target[i] != '?')
		targ += rl.target[i++];
	if (rl.target[i++] == '?')
		while(rl.target[i])
			rl.query += rl.target[i++];
	rl.target = targ;
}

//This returns a t_route with default settings chosen by us.
t_route get_default_route()
{
	t_route route;
	route.allowed_methods.push_back("GET");
	route.allowed_methods.push_back("HEAD");
	route.default_dir_file = "www/File_is_a_dir.html";
	route.dir_listing = false;
	route.root_dir = ".";
	route.upload_root_dir = ".";
	route.modifier = 0;
	route.location = "/";
	route.cgi = false;
	route.auth = false;
	route.body_limit = (size_t)-1;
	return (route);
}

t_route get_route_for(t_request rl, t_server conf)
{
	if (conf.routes.empty())
		return (get_default_route());
	for (std::vector<t_route>::iterator it = conf.routes.begin(); it != conf.routes.end(); it++)
		if ((*it).location == rl.target || (!(*it).modifier && std::string(rl.target,0, (*it).location.length()) == (*it).location))
			return (*it);
	return (get_default_route());
}

bool method_supported(std::string method)
{
	return(method == "GET" || method == "HEAD" || method == "POST" || method == "PUT"
			|| method == "DELETE" || method == "CONNECT" || method == "OPTIONS" || method == "TRACE");
}

bool method_allowed(std::string method, t_route route)
{
	for (std::vector<std::string>::iterator it = route.allowed_methods.begin(); it != route.allowed_methods.end(); it++)
		if (method == *it)
			return (true);
	return (false);
}

//Returns fd to the directory listing
void get_dir_listing(std::string dir)
{
	int fd;
	int r;
	std::string start = "<html>\n<head><title>Index of "+ dir +"</title></head>\n<body bgcolor=\"white\"><h1>Index of " + dir +"</h1><hr>";
	DIR *dptr = opendir(dir.c_str());
	struct dirent *tmp;
	if (dir[dir.length() - 1] != '/')
		dir += "/";
	fd = open(".dirlisting.html", O_CREAT | O_TRUNC | O_RDWR, 0666);//For bonus workers add worker's id to file name
	r = PUT_FILE(fd, start.c_str(), ft_strlen(start.c_str()));
	if (r == -1 || r == 0)
	{
		closedir(dptr);
		close(fd);
		return;
	}
	while ((tmp = readdir(dptr))) //build href for each file
	{
		r = PUT_FILE(fd, "<pre><a href=\"", 14);
		if (r == -1 || r == 0)
		{
			closedir(dptr);
			close(fd);
			return;
		}
		if (std::string(tmp->d_name) == ".")
			r = PUT_FILE(fd, (std::string(dir, 1, dir.length() - 1)).c_str(), dir.length() -1);
		else
			r = PUT_FILE(fd, (std::string(dir, 1, dir.length() - 1)+ std::string(tmp->d_name)).c_str(), ft_strlen(tmp->d_name) + dir.length() -1);
		if (r == -1 || r == 0)
		{
			closedir(dptr);
			close(fd);
			return;
		}
		r = PUT_FILE(fd, "\">", 2);
		if (r == -1 || r == 0)
		{
			closedir(dptr);
			close(fd);
			return;
		}
		r =PUT_FILE(fd, tmp->d_name, ft_strlen(tmp->d_name));
		if (r == -1 || r == 0)
		{
			closedir(dptr);
			close(fd);
			return;
		}
		r = PUT_FILE(fd, "</a></pre>", 10);
		if (r == -1 || r == 0)
		{
			closedir(dptr);
			close(fd);
			return;
		}
	}
	r = PUT_FILE(fd, "<hr></body>\n</html>\n", 20);
	if (r == -1 || r == 0)
	{
		closedir(dptr);
		close(fd);
		return;
	}
	closedir(dptr);
	close(fd);
}

// ex: for PUT /somedir/index.html if somedir doesn't exist create it
void create_missing_dirs(std::string targ, t_route route)
{
	(void)route;
	size_t i = 0;
	std::string target = targ; // later change to upload_root_dir
	while(target[i])
	{
		i++;
		size_t len = 0;
		while (target[i + len] != '/' && target[i + len])
			len++;
		if (!target[i + len])
			return;
		i += len;
		std::string dir_to_create = std::string(target, 0, i );
		if (!file_exists(dir_to_create))
			mkdir(dir_to_create.c_str(), 0777);
	}
}

//Used to create a file with HTTP PUT method
void create_ressource(t_request rl, t_route route, t_response &resp, char **&envp)
{
	int fd;
	std::string ressource_content = rl.body;
	create_missing_dirs(rl.target, route);
	fd = open((rl.target).c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0777); // See above comm
	if (rl.method == "POST")
	{
		ressource_content = execute_cgi(rl, route, resp, envp);
		resp.body = ressource_content;
	}
	int r = PUT_FILE(fd, ressource_content.c_str(), ressource_content.length());
	if (r == -1)
		std::cout << "Warning PUT could not modify content of file currently" << std::endl;
	else if (r == 0) 
		std::cout << "Warning creating file with size of 0. Nothing bad, you're good to go." << std::endl;
	close(fd);
}

void empty_directory(std::string path)
{
	DIR *dptr = opendir(path.c_str());
	struct dirent *tmp;
	while ((tmp = readdir(dptr))) 
	{
		if (!file_is_dir(path + "/" + tmp->d_name))
			unlink((path+ "/" + tmp->d_name).c_str());
	}
	closedir(dptr);
}
