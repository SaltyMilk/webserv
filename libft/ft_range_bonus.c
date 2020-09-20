/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_range_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/15 15:49:26 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/17 13:47:44 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	*ft_range(int min, int max)
{
	int *range;
	int rsize;
	int i;

	rsize = max - min + 1;
	if (!(range = malloc(sizeof(int) * rsize)))
		return (NULL);
	i = 0;
	while (i < rsize)
	{
		range[i] = min + i;
		i++;
	}
	return (range);
}
