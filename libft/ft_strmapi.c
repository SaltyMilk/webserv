/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/09 02:06:22 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/20 18:06:34 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*nstr;
	unsigned int	i;

	if (!s || !f)
		return (NULL);
	if (!(nstr = malloc(ft_strlen(s) + 1)))
		return (NULL);
	i = 0;
	while (s[i])
	{
		nstr[i] = s[i];
		nstr[i] = (*f)(i, nstr[i]);
		i++;
	}
	nstr[i] = '\0';
	return (nstr);
}
