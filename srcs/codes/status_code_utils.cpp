#include "../../includes/webserv.h"

std::string get_allowed_methods(t_route route)
{
	std::string ret;
	size_t i = 0;
	while (i < route.allowed_methods.size())
	{
		if (i + 1 < route.allowed_methods.size())
			ret += (route.allowed_methods[i] + ", ");
		else //last allowed method
			ret += route.allowed_methods[i];
		i++;
	}
	return (format_header(ALLOW, ret));
}