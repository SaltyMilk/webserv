#include "libft.h"

void	ft_memdel(void **ap)
{
	if (ap == NULL)
		return ;
	if (*ap != NULL)
		free(*ap);
	*ap = NULL;
}
