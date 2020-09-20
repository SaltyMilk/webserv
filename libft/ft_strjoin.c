/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/08 15:48:08 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/16 18:47:05 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*jstr;
	int		i;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	if (!(jstr = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char))))
		return (NULL);
	while (*s1)
	{
		jstr[i] = *s1;
		s1++;
		i++;
	}
	while (*s2)
	{
		jstr[i] = *s2;
		s2++;
		i++;
	}
	jstr[i] = '\0';
	return (jstr);
}
