/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 08:44:59 by sel-melc          #+#    #+#             */
/*   Updated: 2019/11/05 19:04:06 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list *clst;

	if (alst && *alst && new)
	{
		clst = *alst;
		while (clst->next)
			clst = clst->next;
		new->next = NULL;
		clst->next = new;
	}
	else if (alst && !(*alst) && new)
	{
		new->next = NULL;
		*alst = new;
	}
}
