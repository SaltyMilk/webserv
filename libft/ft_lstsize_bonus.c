/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 08:47:33 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/14 13:00:41 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int		i;
	t_list	*clst;

	if (!lst)
		return (0);
	i = 0;
	clst = lst;
	while (clst)
	{
		clst = clst->next;
		i++;
	}
	return (i);
}
