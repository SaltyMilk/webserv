#include "../includes/webserv.h"
int		client_count = 0;//Remove once project is finished, good for debugging

int net_init(unsigned int port, std::string host_addr) 
{
	int		fd; //Server's socket 
	struct sockaddr_in	self_adr;

	// open socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		excerr("Couldn't open server socket.", 1);

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

void net_receive(std::vector<t_conf> servers, int client_fd, int server_fd)
{
	char	buff[BUFF_SIZE];
	int ret;
	std::string req;
	ft_bzero(buff, BUFF_SIZE);
	while ((ret = read(client_fd, buff, BUFF_SIZE - 1)) > 0)
	{
		buff[ret] = 0;
		req += buff;
		ft_bzero(buff, sizeof(buff));
	}
	if (req.length())
		parse_request(const_cast<char *>(req.c_str()), client_fd, servers, server_fd);
	else
		close(client_fd);

}

int net_accept(t_net &snet, int fd) 
{
	struct sockaddr_in	client_adr;
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
		excerr("Couldn't unblock client's socket", 1);//Should change this to send 500 internal error in the future
	std::cout << "client accepted with fd=" << fd << std::endl;
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

int main(int argc, char **argv) 
{
	std::vector<t_conf> servers;
	t_net s_net;
	std::string conf_file = "ws.conf"; //Default path
	fd_set sockets, ready_sockets;
	std::vector<int> serv_fds;
	int next_fd_to_resp = -1;

	signal(SIGINT, chandler);
//HANDLE CONFIG FILE
	if (argc == 2)
		conf_file = argv[1]; //Use config file given as arg
	servers = parseConf(conf_file);
//START NETWORKING
	FD_ZERO(&sockets);
	init_all_servers(servers, serv_fds, &sockets);
	while (1)
	{
		ready_sockets = sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) == -1)
			excerr("Select failed.", 1);
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))//Socket ready for w/r operations
			{
				if (std::find(serv_fds.begin(), serv_fds.end(), i) != serv_fds.end())//Connection is being asked to one of the servers
				{
					next_fd_to_resp = i;
					FD_SET(net_accept(s_net, *std::find(serv_fds.begin(), serv_fds.end(), i)), &sockets);//add new client socket to the set fd
				}
				else
				{
					net_receive(servers, i, next_fd_to_resp);
					FD_CLR(i, &sockets);//Remove client socket from list of active sockets after serving him
				}
			}
		}
	}
	return 0;
}