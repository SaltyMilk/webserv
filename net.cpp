#include "net.h"
int		fd; 			// notre socket de connection
int		client_fd[1000];	// la socket du client
char 	client_connected[1000];
int		client_count = 0;

int net_init(int port) 
{
	struct sockaddr_in	self_adr;

	ft_memset(client_connected, 1, 1000);
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

int net_shutdown(void)
{
	// close socket
	if (close(fd) == -1) {
		printf("Couldn't close socket.\n");
		return 1;
	}

	return 0;
}

int net_receive(void)
{
	char	buff[1024];//Must change to dynamic buffer
	int len;

	for (int i = 0; i < client_count; i++)
	{
		len = read(client_fd[i], buff, sizeof(buff) - 1);
		if (len > 0){
			buff[len] = 0;
	//		std::cout << buff;
			if (parse_request(buff, client_fd[i]))
				return (1);
		}
		else if (!len && client_connected[i])
		{
			client_connected[i] = 0;
			std::cout << "Client "<< i << " disconnected" << std::endl;
		}
		else if (errno == 35) // Will have to be removed for pdf
			continue;
		else {
			std::cout << "Couldn't read socket: " << strerror(errno) << ", " << errno << std::endl;
			return 1;
		}
	}
	return 0;
}

int net_accept(void) 
{
	struct sockaddr_in	client_adr;
	unsigned int		len;

	// set client adr len
	len = sizeof(client_adr);

	// accept socket
	if ((client_fd[client_count] = accept(fd, (struct sockaddr *)&client_adr, &len)) == -1) {
		if (errno == 35)
			return 0;
		else {
			printf("Couldn't accept socket: %s, %d.\n", strerror(errno), errno);
			return 1;
		}
	}

	client_count++;
	printf("Client %d connected !\n", client_count-1);

	// unblock socket
	if ((fcntl(client_fd[client_count - 1], F_GETFL, O_NONBLOCK)) == -1)
		return 1;
	return 0;
}

int main(int argc, char **argv) 
{
	(void)argc;
	if (net_init(atoi(argv[1])))
		return 1;
	while (1)
		if (net_accept() || net_receive())
			return 1;
	if (net_shutdown())
		return 1;
	return 0;
}