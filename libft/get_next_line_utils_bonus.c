/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/18 16:45:39 by sel-melc          #+#    #+#             */
/*   Updated: 2019/12/18 18:34:17 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** mod 0 =  eolp | mod 1 = strlen
*/

int		eolp(char *buffer, char mod)
{
	int i;

	i = 0;
	if (mod)
	{
		while (buffer[i])
			i++;
		return (i);
	}
	while (buffer[i])
	{
		if (buffer[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

/*
** mod 0 = del one line | mode 1 = emptybuffer
*/

void	delol(char *tempbuffer, char mod)
{
	long	i;
	long	j;

	i = 0;
	j = 0;
	if (mod)
	{
		while (i < BUFFER_SIZE)
			tempbuffer[i++] = 0;
		tempbuffer[BUFFER_SIZE] = 0;
		return ;
	}
	while (tempbuffer[i] != '\n' && tempbuffer[i])
		i++;
	j = ++i;
	while (tempbuffer[i])
	{
		tempbuffer[i - j] = tempbuffer[i];
		i++;
	}
	tempbuffer[i - j] = '\0';
}

void	fill_tempbuffer(char *tempbuffer, char *buffer, int eolp)
{
	while (buffer[++eolp])
	{
		*tempbuffer = buffer[eolp];
		tempbuffer++;
	}
	*tempbuffer = '\0';
}

char	*ultstrcat(char *s1, char *s2)
{
	char	*nstr;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!(nstr = malloc(eolp(s1, 1) + eolp(s2, 1) + 1)))
		return (NULL);
	while (s1[i])
	{
		nstr[i] = s1[i];
		i++;
	}
	free(s1);
	while (s2[j] && s2[j] != '\n')
		nstr[i++] = s2[j++];
	nstr[i] = '\0';
	return (nstr);
}
