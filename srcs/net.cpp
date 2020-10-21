#include "../includes/webserv.h"
int		client_count = 0;//Remove once project is finished, good for debugging

int net_init(unsigned int port) 
{
	int		fd; //Server's socket 
	struct sockaddr_in	self_adr;

	// open socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		excerr("Couldn't open server socket.", 1);

	ft_memset((void *)&self_adr, 0, sizeof(self_adr));
	self_adr.sin_family = AF_INET;
    self_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
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

void net_receive(t_net &snet, t_conf conf, int client_fd)
{
	char	buff[BUFF_SIZE];//Must change to dynamic buffer
	int ret;
	std::string req;
	int i = 0;
	ft_bzero(buff, BUFF_SIZE);
	while ((ret = read(client_fd, buff, BUFF_SIZE - 1)) > 0 )
	{
		buff[ret] = 0;
		req += buff;
		ft_bzero(buff, sizeof(buff));
	}
	buff[i] = 0;
	req += buff;
	parse_request(const_cast<char *>(req.c_str()), client_fd, snet, conf);
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
	return client_fd;
}

int main(int argc, char **argv) 
{
	std::vector<t_conf> servers;
	t_net s_net;
	std::string conf_file = "ws.conf"; //Default path
	fd_set sockets, ready_sockets;
	int serv_fd;
//HANDLE CONFIG FILE
	if (argc == 2)
		conf_file = argv[1]; //Use config file given as arg
	servers = parseConf(conf_file);
//START NETWORKING
	FD_ZERO(&sockets);
	serv_fd = net_init(servers[0].ports[0]); //Get socket of the serv
	FD_SET(serv_fd, &sockets);//add server socket to the set of fd
	while (1)
	{
		ready_sockets = sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) == -1)
			excerr("Select failed.", 1);
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))//Socket ready for w/r operations
			{
				if (i == serv_fd)//Connection is being asked to the server
					FD_SET(net_accept(s_net, serv_fd), &sockets);//add new client socket to the set fd
				else
				{
					net_receive(s_net, servers[0], i);
					FD_CLR(i, &sockets);
				}
			}
		}
	}
	return 0;
}