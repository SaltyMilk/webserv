/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 08:45:54 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/24 12:14:57 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list *clst;

	if (lst && *lst && del)
	{
		clst = *lst;
		while (clst)
		{
			ft_lstdelone(clst, (*del));
			clst = clst->next;
		}
		*lst = NULL;
	}
}
