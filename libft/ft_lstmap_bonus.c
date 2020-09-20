/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 08:46:58 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/20 14:38:38 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*snewlst;
	t_list	*clst;
	t_list	*tlst;

	if (!lst || !f)
		return (NULL);
	if (!(snewlst = ft_lstnew((*f)(lst->content))))
		return (NULL);
	clst = lst->next;
	while (clst)
	{
		if (!(tlst = ft_lstnew((*f)(clst->content))))
		{
			ft_lstclear(&snewlst, del);
			return (NULL);
		}
		ft_lstadd_back(&snewlst, tlst);
		clst = clst->next;
	}
	return (snewlst);
}
