#include "../includes/webserv.h"
int		fd; 			// notre socket de connection
//int		client_fd[1000];	// la socket du client
int		client_count = 0;

int net_init(int port) 
{
	struct sockaddr_in	self_adr;

	// open socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		return (print_err("Couldn't open socket.\n"));

	ft_memset((void *)&self_adr, 0, sizeof(self_adr));
	self_adr.sin_family = AF_INET;
    self_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    self_adr.sin_port = htons(port);

	// bind socket
	if (bind(fd, (const struct sockaddr *)&self_adr, sizeof(self_adr)) == -1)
		return (print_err("Couldn't bind socket\n"));

	// unblock socket
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		return 1;

	// listen socket
	if (listen(fd, 10) == -1) 
		return (print_err("Couldn't listen socket.\n"));
	return 0;
}

int net_receive(t_net &snet, t_conf conf)
{
	char	buff[BUFF_SIZE];//Must change to dynamic buffer
	int ret;
	std::string req;
	int i = 0;
	ft_bzero(buff, BUFF_SIZE);
	for (std::list<int>::iterator it = snet.client_fds.begin() ; it != snet.client_fds.end(); it++)
	{
		while ((ret = read(*it, buff, BUFF_SIZE - 1)) > 0 )
		{
			buff[ret] = 0;
			req += buff;
			ft_bzero(buff, sizeof(buff));
		}
		buff[i] = 0;
		req += buff;
		if (req.length() > 0)
		{
			if (parse_request(const_cast<char *>(req.c_str()), *it, snet, conf))
				return (1);
			snet.client_fds.remove(*it);
			it = snet.client_fds.begin();
		}
	}
	return 0;
}

int net_accept(t_net &snet) 
{
	struct sockaddr_in	client_adr;
	unsigned int		len;
	int tmp_fd;
	// set client adr len
	len = sizeof(client_adr);
	
	// accept socket
	if ((tmp_fd = accept(fd, (struct sockaddr *)&client_adr, &len)) == -1)
	{
		if (errno == 35)
			return 0;
		else {
			printf("Couldn't accept socket: %s, %d.\n", strerror(errno), errno);
			return 1;
		}
	}
	snet.client_fds.push_back(tmp_fd);

	client_count++;
	printf("Client %d connected !\n", client_count);

	// unblock socket
	if ((fcntl(tmp_fd, F_GETFL, O_NONBLOCK)) == -1)
		return 1;
	return 0;
}

int main(int argc, char **argv) 
{
	t_conf conf;
	t_net s_net;
	ft_bzero(&conf, sizeof(conf));
	std::string conf_file = "ws.conf"; //Default path

	if (argc == 2)
		conf_file = argv[1]; //Use config file given as arg
	conf = parseConf(conf_file);
	if (net_init(conf.ports[0]))
		return 1;
	while (1)
		if (net_accept(s_net) || net_receive(s_net, conf))
			return 1;
	return 0;
}