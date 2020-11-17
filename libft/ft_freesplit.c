#include "libft.h"

void		ft_freesplit(char **split)
{
	size_t	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
		ft_strdel(&split[i++]);
	ft_memdel((void**)split);
}

