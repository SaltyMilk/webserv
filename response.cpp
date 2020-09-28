#include "webserv.h"

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

int getorhead_resp(t_req_line rl, t_http_res &resp)
{
	int fd;
	if ((fd = open(("." + rl.target).c_str(), O_RDONLY)) == -1) //Couldn't find requested file on server
	{
		resp.headers[CONTENT_TYPE] =  ("Content-Type: " + std::string("text/html")); 
		resp.status_code = "404";
		resp.reason_phrase = "Not Found";
		char c;
		int efd = open("404.html", O_RDONLY);
		if (rl.method == "GET") // No body for head method
			while (read(efd, &c, 1) > 0)
				resp.body += c;
	}
	else // Requested file was found.
	{
		resp.headers[CONTENT_TYPE] =  "Content-Type: "+ get_content_type("." + rl.target); //ADD CONTENT_TYPE HEADER TO HTTP RESP (missing charset for now)
		resp.status_code = "200";
		resp.reason_phrase = "OK";
		char c;
		if (rl.method == "GET") // No body for head method
			while (read(fd, &c, 1) > 0)
				resp.body += c;
	}
	return (0);
}

int answer_request(int client_fd, t_req_line rl, t_net &snet)
{
	t_http_res resp;
	std::string response; //This will be sent as a response to a given request
	resp.http_ver = "HTTP/1.1";//We will always respond with the version we use
	//Add date header to all responses
	resp.headers[DATE] = "Date: " + get_imf_fixdate();

	if (rl.method == "GET" || rl.method == "HEAD")
		getorhead_resp(rl, resp);
	response = construct_response(resp);
	//SEND REQUEST
	write(client_fd, response.c_str(), ft_strlen(response.c_str()));
	//REMOVE ClIENT FROM CLIENT LIST AND CLOSE CONNECTION. (Fixs pending requests)
	snet.client_fds.remove(client_fd);
	close(client_fd);
	return (0);
}
