/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/25 14:06:24 by sel-melc          #+#    #+#             */
/*   Updated: 2020/07/25 22:54:01 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static	int		is_in_str(char c, char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
	{
		if (c == str[i])
			return (1);
		i++;
	}
	return (0);
}

static	int		get_strcount(char *str, char *charset)
{
	int i;
	int wc;

	wc = 1;
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i + 1] == '\0' && (!(is_in_str(str[i], charset))))
			wc++;
		else if (is_in_str(str[i], charset))
		{
			if (i != 0 && (!(is_in_str(str[i - 1], charset))))
				wc++;
		}
		i++;
	}
	return (wc - 1);
}

static	int		*get_char_str(int strcount, char *str, char *charset)
{
	int *count;
	int ijcc[3];

	if (!(count = malloc(sizeof(int) * (strcount))))
		return (NULL);
	ijcc[0] = 0;
	ijcc[1] = 0;
	ijcc[2] = 0;
	while (str[ijcc[0]] != '\0' && ijcc[1] < strcount)
	{
		if (str[ijcc[0] + 1] == '\0' && (!(is_in_str(str[ijcc[0]], charset))))
			count[ijcc[1]] = ++ijcc[2];
		else if (is_in_str(str[ijcc[0]], charset))
		{
			if (ijcc[0] != 0 && (!(is_in_str(str[ijcc[0] - 1], charset))))
			{
				count[ijcc[1]++] = ijcc[2];
				ijcc[2] = 0;
			}
		}
		else
			ijcc[2]++;
		ijcc[0]++;
	}
	return (count);
}

static	void	ft_split_dep(int *count, char **strs_t, char *str, char *chset)
{
	int i;
	int j;
	int k;

	i = 0;
	j = 0;
	k = 0;
	while (str[k] != '\0')
	{
		if (j == count[i])
		{
			strs_t[i][j] = '\0';
			j = 0;
			i++;
		}
		if (!(is_in_str(str[k], chset)))
		{
			strs_t[i][j] = str[k++];
			j++;
		}
		while (is_in_str(str[k], chset))
			k++;
	}
	strs_t[i][j] = '\0';
	strs_t[i + 1] = 0;
}

char			**ft_strtok(char *str, char *charset)
{
	char		**strs_t;
	int			*count;
	int			i;

	i = 0;
	if (!(get_strcount(str, charset)))
	{
		if (!(strs_t = malloc(sizeof(char *))))
			return (NULL);
		strs_t[0] = 0;
		return (strs_t);
	}
	if (!(strs_t = malloc(sizeof(char*) * (get_strcount(str, charset) + 1)))
		|| !(count = get_char_str(get_strcount(str, charset), str, charset)))
		return (NULL);
	i = 0;
	while (i < get_strcount(str, charset))
	{
		if (!(strs_t[i] = malloc(sizeof(char) * (count[i] + 1))))
			return (NULL);
		i++;
	}
	ft_split_dep(count, strs_t, str, charset);
	free(count);
	return (strs_t);
}