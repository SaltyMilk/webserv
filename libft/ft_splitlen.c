#include "libft.h"

size_t	ft_splitlen(char **split)
{
	size_t i;

	i = 0;
	while (split[i])
		i++;
	return (i);
}