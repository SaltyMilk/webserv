#include "webserv.h"

int index_requested(t_req_line rl, t_http_res &resp, t_conf conf)
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
			send_403(rl, resp);
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

int getorhead_resp(t_req_line rl, t_http_res &resp, t_conf conf)
{
	int fd;
	if (rl.target == "/" && !index_requested(rl, resp, conf)) //Use webserv's index for target
		return (0);
	if ((fd = open(("." + rl.target).c_str(), O_RDONLY)) == -1) //Couldn't find requested file on server
		send_404(rl, resp);
	else // Requested file was found.
		send_200(rl, resp, fd);
	return (0);
}

int answer_request(int client_fd, t_req_line rl, t_net &snet, t_conf conf)
{
	t_http_res resp;
	std::string response; //This will be sent as a response to a given request
	resp.http_ver = "HTTP/1.1";//We will always respond with the version we use
	//Add server header to all responses
	resp.headers[SERVER] = "Server: webserv/" + std::string(WEBSERV_VER);
	//Add date header to all responses
	resp.headers[DATE] = "Date: " + get_imf_fixdate();
	if (bad_request(rl))
		send_400(rl, resp);
	else if (!valid_http_ver(rl)) //SEND 505 to invalid HTTP VERSION REQUEST
		send_505(rl, resp);
	else if (rl.method == "GET" || rl.method == "HEAD")
		getorhead_resp(rl, resp, conf);
	response = construct_response(resp);
	write(client_fd, response.c_str(), ft_strlen(response.c_str()));
	//REMOVE ClIENT FROM CLIENT LIST AND CLOSE CONNECTION. (Fixs pending requests)
	snet.client_fds.remove(client_fd);
	close(client_fd);
	return (0);
}
