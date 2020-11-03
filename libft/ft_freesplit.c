#include "libft.h"

static void	ft_memdel(void **ap)
{
	if (ap == NULL)
		return ;
	if (*ap != NULL)
		free(*ap);
	*ap = NULL;
}

static void	ft_strdel(char **as)
{
	ft_memdel((void **)as);
}

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

