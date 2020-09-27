#include <iostream>
int print_err(std::string s)
{
	std::cout << s;
	return (1);
}
//Cerr msg and exits with code c
void excerr(std::string msg, int c)
{
	std::cerr << msg << std::endl;
	exit(c);
}