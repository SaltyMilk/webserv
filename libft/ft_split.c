/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/08 16:47:58 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/21 15:45:18 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int		is_onlyc_after(const char *cp, char c)
{
	cp++;
	while (*(cp))
	{
		if (*cp != c)
			return (0);
		cp++;
	}
	return (1);
}

static int		get_word_count(char const *s, char c)
{
	int wc;
	int flag;

	flag = 0;
	wc = 0;
	while (*s)
	{
		if (*s == c && *(s + 1) != '\0' && !is_onlyc_after(s, c) && flag)
		{
			wc++;
			flag = 0;
		}
		else if (*s != c)
			flag = 1;
		if (*(s + 1) == '\0')
			wc++;
		s++;
	}
	return (wc);
}

static int		*get_char_count(char const *s, char c, int wc)
{
	int *cc;
	int flag;
	int i;

	if (!(cc = malloc(sizeof(int) * wc)))
		return (NULL);
	i = 0;
	cc[0] = 0;
	flag = 0;
	while (*s)
	{
		if (*s == c && *(s + 1) != '\0' && !is_onlyc_after(s, c) && flag)
		{
			i++;
			cc[i] = 0;
			flag = 0;
		}
		else if (*s != c)
		{
			flag = 1;
			cc[i]++;
		}
		s++;
	}
	return (cc);
}

static void		split_dep(char const *s, char **strs, char c)
{
	int ijf[3];

	ft_bzero(ijf, 3 * sizeof(int));
	if (!(*s) || is_onlyc_after(s, c))
		strs[0] = NULL;
	else
	{
		while (*s)
		{
			if (*s == c && *(s + 1) != '\0' && !is_onlyc_after(s, c) && ijf[2])
			{
				strs[ijf[0]++][ijf[1]] = '\0';
				ijf[1] = 0;
				ijf[2] = 0;
			}
			else if (*s != c)
			{
				strs[ijf[0]][ijf[1]++] = *s;
				ijf[2] = 1;
			}
			s++;
		}
		strs[ijf[0]][ijf[1]] = '\0';
		strs[++ijf[0]] = NULL;
	}
}

char			**ft_split(char const *s, char c)
{
	char	**strs;
	int		wci[2];
	int		*cc;

	if (!s)
		return (NULL);
	wci[0] = get_word_count(s, c);
	wci[1] = -1;
	if (!(strs = (char **)malloc(sizeof(char *) * (wci[0] + 1))))
		return (NULL);
	if (!(cc = get_char_count(s, c, wci[0])))
		return(NULL);
	while (++wci[1] < wci[0])
		if (!(strs[wci[1]] = malloc(sizeof(char) * (cc[wci[1]] + 1))))
		{
			free(cc);
			if (wci[1])
				while (--wci[1] >= 0)
					free(strs[wci[1]]);
			return (NULL);
		}
	split_dep(s, strs, c);
	free(cc);
	return (strs);
}
