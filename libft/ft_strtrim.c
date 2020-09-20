/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/12 00:36:32 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/22 20:58:27 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_in_set(char c, char const *set)
{
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

static int	is_onlyset_cafter(const char *cp, const char *set)
{
	while (*(cp))
	{
		if (!is_in_set(*cp, set))
			return (0);
		cp++;
	}
	return (1);
}

static int	trimd_strsize(char const *s1, char const *set)
{
	int trimc;
	int i_len;
	int i;
	int f_len;

	i = 0;
	trimc = 0;
	i_len = ft_strlen(s1) - 1;
	f_len = i_len + 1;
	while (s1[i] && is_in_set(s1[i], set))
	{
		trimc++;
		i++;
	}
	while (s1[i_len] && is_in_set(s1[i_len], set))
	{
		trimc++;
		i_len--;
	}
	return (f_len - trimc);
}

static void	strtrim_dep(char const *s1, char const *set, char *strimd)
{
	int i;
	int j;

	i = 0;
	j = 0;
	while (is_in_set(s1[i], set))
		i++;
	while (s1[i])
	{
		if (is_onlyset_cafter((s1 + i), set))
			break ;
		else
			strimd[j++] = s1[i];
		i++;
	}
	strimd[j] = '\0';
}

char		*ft_strtrim(char const *s1, char const *set)
{
	char *strimd;

	if (!s1 || !set)
		return (NULL);
	if (is_onlyset_cafter(s1, set))
	{
		if (!(strimd = malloc(1)))
			return (NULL);
		*strimd = '\0';
		return (strimd);
	}
	if (!(strimd = malloc(trimd_strsize(s1, set) + 1)))
		return (NULL);
	strtrim_dep(s1, set, strimd);
	return (strimd);
}
