#include "webserv.h"

std::string get_allow_header_for(t_route route)
{
	std::string ret = "Allow: ";
	size_t i = 0;
	while (i < route.allowed_methods.size())
	{
		if (i + 1 < route.allowed_methods.size())
			ret += (route.allowed_methods[i] + ", ");
		else //last allowed method
			ret += route.allowed_methods[i];
		i++;
	}
	return (ret);
}