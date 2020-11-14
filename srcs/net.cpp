#include "../includes/webserv.h"
int		client_count = 0;//Remove once project is finished, good for debugging
std::vector<int> 	serv_socket;

std::string cinet_ntoa(in_addr_t in)
{
	unsigned char *bytes = (unsigned char *) &in;
	std::string ret = "";
	for (int i = 0; i < 4; i++)
	{
		ret += std::to_string(bytes[i]);
		if (i != 3)
			ret+= ".";
	}
	return ret;
}
int net_init(unsigned int port, std::string host_addr)
{
	int		fd; //Server's socket
	struct sockaddr_in	self_adr;

	// open socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		excerr("Couldn't open server socket.", 1);

	serv_socket.push_back(fd);
	ft_memset((void *)&self_adr, 0, sizeof(self_adr));
	self_adr.sin_family = AF_INET;
    self_adr.sin_addr.s_addr = inet_addr(host_addr.c_str());
	uint16_t goodport = (port>>8) | (port<<8); //replaces htons
    self_adr.sin_port = goodport;

	// bind socket
	if (bind(fd, (const struct sockaddr *)&self_adr, sizeof(self_adr)) == -1)
		excerr("Couldn't bind socket", 1);

	// unblock socket
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		excerr("Couldn't unblock server socket", 1);

	// listen socket
	if (listen(fd, 10) == -1) 
		excerr("Couldn't listen on server socket.", 1);
	return fd;
}

t_ans_arg net_receive(std::vector<t_conf> servers, int client_fd, int server_fd, const struct sockaddr_in	client_adr, char **envp)
{
	std::cout << "starting to receive" << std::endl;
	char	buff[BUFF_SIZE];
	int ret;
	std::string req;
	ft_bzero(buff, BUFF_SIZE);
	while ((ret = recv(client_fd, buff, BUFF_SIZE - 1, 0)))
	{
		if (errno == EAGAIN)
		{
			usleep(4200);
			ret = recv(client_fd, buff, BUFF_SIZE - 1, 0);
			if (errno == EAGAIN)
				break;
		}
		buff[ret] = 0;
		req += buff;
		ft_bzero(buff, sizeof(buff));
	}
	if (ret == 0)
	{
		std::cout << "MAN WTF RECV RETURNED 0" << std::endl;
		req += buff;
	}
	if (ret == -1)
		std::cout << "errno=" << strerror(errno) << std::endl;
			std::cout <<  "DEBUG BEGINING OF REQUEST OUTPUT" << std::endl;
//	size_t k = 0;
	std::cout << "done receiving" << std::endl;
	if (req.length() < 2000)
		std::cout << req << std::endl;
	/*while (req[k] && !(req[k] == '\r' && req[k + 1] == '\n' && req[k+2] == '\r' && req[k +3] == '\n'))
		write(1, req.c_str() + k++, 1);*/
	std::cout <<std::endl << "DEBUG END OF REQUEST OUTPUT" << std::endl;
	//std::cout << "REQUEST LOG" << std::endl << req << std::endl << "END REQUEST LOG" << std::endl;
	if (req.length())
		return parse_request(const_cast<char *>(req.c_str()), client_fd, servers, server_fd, client_adr, envp);
	else
	{
		std::cout << "WTF WHY DID I GET HERE ?" << std::endl;
		close(client_fd);
	}
	//Doesn't matter we won't answer this
	t_ans_arg arg;
	return(arg);
}

int net_accept(t_net &snet, int fd, struct sockaddr_in	&client_adr) 
{
	unsigned int		len;
	int client_fd;
	len = sizeof(client_adr);
	
	// accept socket
	if ((client_fd = accept(fd, (struct sockaddr *)&client_adr, &len)) == -1)
			excerr("Couldn't accept connection", 1);
	client_count++;
	std::cout << "Client " << client_count << " connected!" << std::endl;
	snet.clients_net.push_back(client_adr);
	// unblock socket
	if ((fcntl(client_fd, F_GETFL, O_NONBLOCK)) == -1)
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
	for (std::vector<t_conf>::iterator it = servers.begin(); it != servers.end(); it++)//Init a/multiple sockets for each server
	{
		for (std::vector<int>::iterator itp = (*it).ports.begin(); itp != (*it).ports.end(); itp++)//Init a socket for a each port
		{
			std::vector<t_hpf>::iterator ret;
			if ((ret = is_in_hpfs((*it).host, *itp, hpfs)) != hpfs.end())//HOST/PORT ALREADY HAS A SOCKET
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
		std::cout <<"sockets=" << *(unsigned int*)sockets.fds_bits << std::endl;
		std::cout << "selecting" << std::endl;
		if (select(max_fd +1, &ready_sockets, &ready_wsockets, NULL, NULL) == -1)
			excerr("Select failed.", 1);
		std::cout << "done selecting" << std::endl;
		for (int i = 0; i <= max_fd; i++)
		{
			if (FD_ISSET(i, &ready_sockets))//Socket ready for read operations
			{
				struct sockaddr_in	client_adr;	
				if (std::find(serv_fds.begin(), serv_fds.end(), i) != serv_fds.end())//Connection is being asked to one of the servers
				{
					next_fd_to_resp = i;
					int fd = net_accept(s_net, *std::find(serv_fds.begin(), serv_fds.end(), i), client_adr);
					FD_SET(fd, &sockets);//add new client socket to the set fd
					FD_SET(fd, &wsockets);//add new client socket to the set fd
					if (fd > max_fd)
						max_fd = fd;
				}
				else//
				{
					requests.push_back(net_receive(servers, i, next_fd_to_resp, client_adr, envp));
					FD_CLR(i, &sockets);//Remove client socket from list of active sockets after serving him
				}
			}
			if (FD_ISSET(i, &ready_wsockets))//Socket ready for write operations
			{
				for (std::vector<t_ans_arg>::iterator it = requests.begin(); it != requests.end(); it++)
					if ((*it).client_fd == i)
					{
						answer_request((*it).client_fd, (*it).rl, (*it).conf, (*it).envp);
						requests.erase(it);
						FD_CLR(i, &wsockets);
						break;
					}
			}
		}
	}
	return (0);
}