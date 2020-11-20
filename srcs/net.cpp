#include "../includes/webserv.h"
int client_count = 0; //Remove once project is finished, good for debugging
std::vector<int> serv_socket;

std::string cinet_ntoa(in_addr_t in)
{
	unsigned char *bytes = (unsigned char *)&in;
	std::string ret = "";
	for (int i = 0; i < 4; i++)
	{
		ret += std::to_string(bytes[i]);
		if (i != 3)
			ret += ".";
	}
	return ret;
}
int net_init(unsigned int port, std::string host_addr)
{
	int fd; //Server's socket
	struct sockaddr_in self_adr;

	// open socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		excerr("Couldn't open server socket.", 1);

	serv_socket.push_back(fd);
	ft_memset((void *)&self_adr, 0, sizeof(self_adr));
	self_adr.sin_family = AF_INET;
	self_adr.sin_addr.s_addr = inet_addr(host_addr.c_str());
	uint16_t goodport = (port >> 8) | (port << 8); //replaces htons
	self_adr.sin_port = goodport;
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		excerr("setsockopt failed.", 1);

	// bind socket
	if (bind(fd, (const struct sockaddr *)&self_adr, sizeof(self_adr)) == -1)
		excerr("Couldn't bind socket", 1);

	// unblock socket
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		excerr("Couldn't unblock server socket", 1);

	// listen socket
	if (listen(fd, 100) == -1)
		excerr("Couldn't listen on server socket.", 1);
	return fd;
}

t_ans_arg net_receive(std::vector<t_conf> servers, int client_fd, int server_fd, const struct sockaddr_in client_adr, char **envp, t_client_buff &cl_buff)
{
	t_ans_arg arg;
	std::cout << "starting to receive" << std::endl;
	char buff[BUFF_SIZE];
	int ret;

	/*if (cl_buff.sec_since_recv && get_time_sec() - cl_buff.sec_since_recv >= TIMEOUT_SEC) //timeout reached before we received a complete HTTP request
	{
		arg.rl.bad_request = true;
		arg.incomplete = false;
		return (arg);
	}*/
	cl_buff.sec_since_recv = get_time_sec();
	ret = recv(client_fd, buff, BUFF_SIZE - 1, 0);
	if (ret == 0)
	{
		std::cout << "returned 0" << std::endl;
		arg.incomplete = true;
		return (arg);
	}
	if (ret == -1)
	{
		arg.incomplete = true;
		std::cout << "errno=" << strerror(errno) << std::endl;
		return (arg);
	}
	std::cout << "read size =" << ret << std::endl;
	buff[ret] = 0;
	cl_buff.req_buff += buff;
	cl_buff.req_buff_len += ret;
	if (cl_buff.rl_set || cl_buff.req_buff.find("\r\n\r\n") != std::string::npos) //Received all headers (can be optimized)
	{
		std::cout << "Received all headers" << std::endl;

		t_req_line rl;
		size_t i = 0;
		char **env = NULL;
		if (!cl_buff.rl_set)
		{
			parse_request_line(i, rl, cl_buff.req_buff.c_str());
			parse_headers(i, rl, cl_buff.req_buff.c_str(), env);
			cl_buff.rl = rl;
			cl_buff.rl_set = true;
			std::cout << "Debug request:" << std::endl
					  << cl_buff.req_buff << std::endl;
		}
		if (env)
		{
			for (size_t j = 0; env[j]; j++)
				free(env[j]);
			free(env);
		}
		if ((!cl_buff.rl.headers[CONTENT_LENGTH][0] || ft_atoi(cl_buff.rl.headers[CONTENT_LENGTH].c_str()) == 0) && !cl_buff.rl.headers[TRANSFER_ENCODING][0]) //No body to expect
		{
			arg = parse_request(const_cast<char *>(cl_buff.req_buff.c_str()), client_fd, servers, server_fd, client_adr, envp);
			std::cout << "yup got here" << std::endl;
			arg.incomplete = false; // DONE RECEIVING REQUEST
			return (arg);
		}
		else //We have to check that we received CONTENT_LENGTH bytes or chunk encoding is completly received
		{
			if (std::string(ft_strlowcase(const_cast<char *>(cl_buff.rl.headers[TRANSFER_ENCODING].c_str()))) == "chunked")
			{
				//THIS MUST BE OPTIMIZED
				//	if (cl_buff.req_buff.find("\r\n\r\n") != cl_buff.req_buff.rfind("\r\n\r\n"))//CRLF at the end of trailer part detected : received all chunks
				if (cl_buff.req_buff.find("\r\n\r\n") < (size_t)cl_buff.req_buff_len - 4 && cl_buff.req_buff[cl_buff.req_buff_len - 4] == '\r' && cl_buff.req_buff[cl_buff.req_buff_len - 3] == '\n' && cl_buff.req_buff[cl_buff.req_buff_len - 2] == '\r' && cl_buff.req_buff[cl_buff.req_buff_len - 1] == '\n')
				{
					arg = parse_request(const_cast<char *>(cl_buff.req_buff.c_str()), client_fd, servers, server_fd, client_adr, envp);
					arg.incomplete = false;
					return (arg);
				}
				else
				{
					arg.incomplete = true;
					return (arg); //Didn't receive all chunks
				}
			}
			else if (ft_atoi(cl_buff.rl.headers[CONTENT_LENGTH].c_str()))
			{
				if (cl_buff.req_buff_len == ft_atoi(cl_buff.rl.headers[CONTENT_LENGTH].c_str())) //Everything received
				{
					arg = parse_request(const_cast<char *>(cl_buff.req_buff.c_str()), client_fd, servers, server_fd, client_adr, envp);
					arg.incomplete = false;
					return (arg);
				}
				else
				{
					arg.incomplete = true;
					return (arg); //Didn't receive all chunks
				}
			}
		}
	}
	std::cout << "done receiving" << std::endl;
	arg.incomplete = true;
	return (arg);
}

int net_accept(t_net &snet, int fd, struct sockaddr_in &client_adr)
{
	unsigned int len;
	int client_fd;
	len = sizeof(client_adr);

	// accept socket
	if ((client_fd = accept(fd, (struct sockaddr *)&client_adr, &len)) == -1)
		excerr("Couldn't accept connection", 1);
	client_count++;
	std::cout << "Client " << client_count << " connected!" << std::endl;
	snet.clients_net.push_back(client_adr);
	// unblock socket
	if ((fcntl(client_fd, F_SETFL, O_NONBLOCK)) == -1)
		excerr("Couldn't unblock client's socket", 1);
	std::cout << "client accepted with fd=" << client_fd << std::endl;
	return client_fd;
}

std::vector<t_hpf>::iterator is_in_hpfs(std::string host, int port, std::vector<t_hpf> hpfs)
{
	std::vector<t_hpf>::iterator it;
	for (it = hpfs.begin(); it != hpfs.end(); it++)
		if ((*it).host == host && (*it).port == port)
			return it;
	return it;
}

void init_all_servers(std::vector<t_conf> &servers, std::vector<int> &serv_fds, fd_set *sockets)
{
	std::vector<t_hpf> hpfs;
	for (std::vector<t_conf>::iterator it = servers.begin(); it != servers.end(); it++) //Init a/multiple sockets for each server
	{
		for (std::vector<int>::iterator itp = (*it).ports.begin(); itp != (*it).ports.end(); itp++) //Init a socket for a each port
		{
			std::vector<t_hpf>::iterator ret;
			if ((ret = is_in_hpfs((*it).host, *itp, hpfs)) != hpfs.end()) //HOST/PORT ALREADY HAS A SOCKET
				(*it).fd.push_back((*ret).fd);
			else
			{
				int tmp_fd = net_init(*itp, (*it).host);
				serv_fds.push_back(tmp_fd);
				(*it).fd.push_back(tmp_fd);
				t_hpf hpf;
				hpf.host = (*it).host;
				hpf.port = *itp;
				hpf.fd = tmp_fd;
				FD_SET(tmp_fd, sockets);
			}
		}
	}
}

int main(int argc, char **argv, char **envp)
{
	std::vector<t_conf> servers;
	t_net s_net;
	std::string conf_file = "ws.conf"; //Default path
	fd_set sockets, ready_sockets;
	fd_set wsockets, ready_wsockets;
	std::vector<int> serv_fds;
	std::vector<t_ans_arg> requests;
	std::vector<t_client_buff> client_buffs;
	int max_fd = 0;
	int next_fd_to_resp = -1;

	signal(SIGINT, chandler);
	//HANDLE CONFIG FILE
	if (argc == 2)
		conf_file = argv[1]; //Use config file given as arg
	servers = parseConf(conf_file);
	//START NETWORKING
	FD_ZERO(&sockets);
	FD_ZERO(&wsockets);
	init_all_servers(servers, serv_fds, &sockets);
	//Find biggest socket from serv fds
	std::cout << "max sock=" << max_fd << std::endl;
	for (std::vector<int>::iterator it = serv_fds.begin(); it != serv_fds.end(); it++)
		if (*it > max_fd)
			max_fd = *it;
	std::cout << "max sock=" << max_fd << std::endl;
	while (1)
	{
		ready_sockets = sockets;
		ready_wsockets = wsockets;
		std::cout << "selecting" << std::endl;
		if (select(max_fd + 1, &ready_sockets, &ready_wsockets, NULL, NULL) == -1)
			excerr("Select failed.", 1);
		std::cout << "done selecting" << std::endl;
		for (int i = 0; i <= max_fd; i++)
		{

			if (FD_ISSET(i, &ready_sockets) && requests.empty()) //Socket ready for read operations
			{
				struct sockaddr_in client_adr;
				if (std::find(serv_fds.begin(), serv_fds.end(), i) != serv_fds.end()) //Connection is being asked to one of the servers
				{
					next_fd_to_resp = i;
					int fd = net_accept(s_net, *std::find(serv_fds.begin(), serv_fds.end(), i), client_adr);
					std::cout << "accepted client with fd = " << fd << std::endl;
					//add a client_buff to receive request
					t_client_buff tmp_cl_buff;
					tmp_cl_buff.client_fd = fd;
					tmp_cl_buff.req_buff = "";
					tmp_cl_buff.sec_since_recv = 0;
					tmp_cl_buff.req_buff_len = 0;
					tmp_cl_buff.rl_set = false;
					client_buffs.push_back(tmp_cl_buff);
					FD_SET(fd, &sockets); //add new client socket to the set fd
					if (fd > max_fd)	  //For optimization select only checks the maximum_fd we are communicating with
						max_fd = fd;
				}
				else //
				{
					std::vector<t_client_buff>::iterator it;
					for (it = client_buffs.begin(); it != client_buffs.end(); it++)
						if (i == (*it).client_fd)
							break;
					t_ans_arg ans_arg = net_receive(servers, i, next_fd_to_resp, client_adr, envp, *it);
					if (ans_arg.incomplete == false) //Done receiving from socket
					{
						//			std::cout <<"this is your request;"<<std::endl <<(*it).req_buff << std::endl;
						std::cout << "received full request" << std::endl;
						client_buffs.erase(it);
						ans_arg.resp_byte_sent = 0;
						requests.push_back(ans_arg);
						FD_CLR(i, &sockets);  //Remove client socket from list of active sockets after serving him
						FD_SET(i, &wsockets); //add new client socket to the set fd
					}
				}
			}
			if (FD_ISSET(i, &ready_wsockets) && !requests.empty()) //Socket ready for write operations
			{
				for (std::vector<t_ans_arg>::iterator it = requests.begin(); it != requests.end(); it++)
					if ((*it).client_fd == i)
					{
						int ret_send = 0;
						if (!(*it).resp_byte_sent)
						{
							(*it).request = answer_request((*it).client_fd, (*it).rl, (*it).conf, (*it).envp);
							(*it).response_length = (*it).request.length();
						}
						if ((*it).response_length <= WRITE_SIZE)
						{
							ret_send = send((*it).client_fd, (*it).request.c_str(), (*it).response_length, 0);
							if (ret_send == 0 || ret_send == -1)
								std::cout << "wtf send" << std::endl;
							else
								(*it).resp_byte_sent += (size_t)ret_send;
						}
						if ((*it).resp_byte_sent == (*it).response_length) //Response fully transfered
						{
						close(i);
						requests.erase(it);
						std::cout << "write block fd(i)=" << i << std::endl;
						FD_CLR(i, &wsockets);
						break;
						}
						else if ((*it).resp_byte_sent < (*it).response_length)//SENDING PART OF THE REP
						{
							if ((*it).response_length - (*it).resp_byte_sent < WRITE_SIZE)
								ret_send = send((*it).client_fd, (*it).request.c_str() + (*it).resp_byte_sent, (*it).response_length - (*it).resp_byte_sent, 0);
							else if ((*it).resp_byte_sent < (*it).response_length - WRITE_SIZE)
								ret_send = send((*it).client_fd, (*it).request.c_str() + (*it).resp_byte_sent, WRITE_SIZE, 0);
							if (ret_send == 0 || ret_send == -1)
								std::cout << "wtf send" << std::endl;
							else
								(*it).resp_byte_sent += (size_t)ret_send;
							break;
						}
						else if ((*it).resp_byte_sent < (*it).response_length)//SEND FULL REQUEST AT ONCE
						{
							ret_send = send((*it).client_fd, (*it).request.c_str(), (*it).response_length, 0);
							if (ret_send == 0 || ret_send == -1)
								std::cout << "wtf send" << std::endl;
							else
								(*it).resp_byte_sent += (size_t)ret_send;
						
							break;
						}

					}
			}
		}
	}
	return (0);
}