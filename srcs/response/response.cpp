#include "../../includes/webserv.h"

int index_requested(t_req_line &rl, t_http_res &resp, t_conf conf)
{
		bool found = false;
		for (size_t i = 0; conf.indexs.size() && i < conf.indexs.size(); i++)
			if (file_exists("./" + conf.indexs[i]))
			{
				rl.target += conf.indexs[i];//tmp will have to try each index later
				found = true;
				break;
			}
		if (!found || !conf.indexs.size())
		{
			send_403(rl, resp, conf);
			return (0);	
		}
	return (1); //index found continue to send_200 execution
}

std::string construct_response(t_http_res resp)
{
	std::string response;
	//CONSTRUCT STATUS LINE
	response = (resp.http_ver + " " + resp.status_code + " " + resp.reason_phrase + "\r\n");
	//ADDING HEADERS
	for (size_t i = 0; i < 18; i++)
		if (resp.headers[i].length() > 0)
			response += (resp.headers[i] + "\r\n");
	response += "\r\n"; // emmpty line before msg
	response += resp.body;
	return (response);
}

int getorhead_resp(t_req_line rl, t_http_res &resp, t_conf conf, t_route route)
{
	int fd;
	if (rl.target == "/" && conf.indexs.size() > 0 && !index_requested(rl, resp, conf)) //Use webserv's index for target
		return (0);
	if (file_is_dir(rl.target) && !route.dir_listing)//Handle directories without dir_listing
		rl.target = rl.target + "/" + route.default_dir_file;
	if (file_is_dir(rl.target) && route.dir_listing)//Handle dir listing
	{
		get_dir_listing(rl.target);
		send_200_dirlist(rl, resp);
	}
	else if ((fd = open((rl.target).c_str(), O_RDONLY)) == -1) //Couldn't find requested file on server
		send_404(rl, resp, conf);
	else // Requested file was found.
		send_200(rl, resp, fd, route);
	return (0);
}

void put_resp(t_req_line rl, t_http_res &resp, t_route route)
{
	rl.target = str_replace(rl.target, route.root_dir, route.upload_root_dir);
	if (file_exists(rl.target))
		send_204_put(rl, resp, route);
	else
		send_201_put(rl, resp);
	create_ressource(rl, route);
}

void delete_resp(t_req_line rl, t_http_res &resp, t_conf conf)
{
	if (!file_exists(rl.target))
		send_404(rl, resp, conf);
	else
	{
		if (file_is_dir(rl.target))
		{
			empty_directory(rl.target);
			rmdir((rl.target).c_str());
		}
		else
			unlink((rl.target).c_str());
		send_204_delete(resp);
	}
}

void post(t_req_line rl, t_http_res &resp, t_route route, t_conf conf)
{
	int fd;

	if ((fd = open((rl.target).c_str(), O_RDONLY)) == -1) //Couldn't find requested file on server might need change, nginx works like this though
		send_404(rl, resp, conf);
	else // Requested file was found.
		send_200(rl, resp, fd, route);

}

int answer_request(int client_fd, t_req_line rl, t_conf conf)
{	
	t_route route;//Settings for requested ressource location
	t_http_res resp;
	std::string response; //This will be sent as a response to a given request
	
	resp.http_ver = "HTTP/1.1";//We will always respond with the version we use
	//Add server header to all responses
	resp.headers[SERVER] = "Server: webserv/" + std::string(WEBSERV_VER);
	//Add date header to all responses
	resp.headers[DATE] = "Date: " + get_imf_fixdate();
	handle_absolute_path(rl);
	parse_query_from_target(rl);//REQ.TARGET IS NOW CLEAN
	route = get_route_for(rl, conf);
	std::cout << "I've chosen this route for you:" << route.location << std::endl;
	if (route.location == "/" && route.root_dir == ".")
		route.root_dir = "./";
	if (!method_allowed(rl.method, route))//Method requested not allowed for requested route/location
		send_405(rl, resp, conf, route);
	else if (bad_request(rl) || rl.bad_request)
		send_400(rl, resp, conf);
	else if (rl.body.length() > conf.body_limit)//Request body was too large for server
		send_413(rl, resp, conf);
	else if (!valid_http_ver(rl)) //SEND 505 to invalid HTTP VERSION REQUEST
		send_505(rl, resp, conf);
	else // REQUEST SHOULD BE VALID NOW AND READY FOR PROCESSING
	{
		rl.target = str_replace(rl.target, route.location, route.root_dir);//Change location in target to root_dir
		if (route.cgi)
			parse_cgi(rl);
		std::cout << "target=" << rl.target << std::endl;
		if (route.auth && (rl.auth.type.empty() || rl.auth.ident.empty()) && route.auth_user != rl.auth.ident)
			send_401(rl, resp, conf, route.auth_name);
		else if (!method_supported(rl.method))//None standard http method requested
			send_501(rl, resp, conf);
		else if (rl.method == "GET" || rl.method == "HEAD")
			getorhead_resp(rl, resp, conf, route);
		else if (rl.method == "PUT")
			put_resp(rl, resp, route);
		else if (rl.method == "DELETE")
			delete_resp(rl, resp, conf);
		else if (rl.method == "POST")
			post(rl, resp, route, conf);
	}
	if (!resp.headers[TRANSFER_ENCODING].length() && resp.status_code[0] != '1' && resp.status_code != "204")//CONTENT_LENGTH HEADER
		resp.headers[CONTENT_LENGTH] = "Content-Length: " + std::to_string(resp.body.length());
	response = construct_response(resp);
	std::cout << "Response status code=" << resp.status_code << std::endl;
//	std::cout << "RESPONSE LOG" << std::endl << response << std::endl << "REPSONSE LOG END" <<std::endl;
	int ret = 0;
	
	if (response.length() > 1000)
	{
		size_t i = 0;//number of bytes written
		while (i < response.length() - 1000)
		{
			write(client_fd, response.c_str() + i, 1000);
			i += 1000;
		}
		if (response.length() - i)
			write(client_fd, response.c_str() + i, response.length() - i);
		ret = i;
	}
	else 
		ret = write(client_fd, response.c_str(), ft_strlen(response.c_str()));
	std::cout << "resp length= " << response.length() << std::endl;
	std::cout << ret << " bytes written" << std::endl;
	//CLOSE CONNECTION. (Fixs pending requests)
	close(client_fd);
	return (0);
}
