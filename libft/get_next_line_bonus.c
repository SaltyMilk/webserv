/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/18 18:30:15 by sel-melc          #+#    #+#             */
/*   Updated: 2019/12/19 17:54:43 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	clean_exit(char **buffer, int ret)
{
	free(*buffer);
	return (ret);
}

int	prep_gnl(int fd, char **line, char **buffer,
			char fdbuffer[_SC_OPEN_MAX][BUFFER_SIZE + 1])
{
	if (fd < 0 || BUFFER_SIZE <= 0 || !line || fd > _SC_OPEN_MAX)
		return (-1);
	if (!(*buffer = malloc((BUFFER_SIZE + 1) * sizeof(char))))
		return (-1);
	if (!(*line = malloc(1 * sizeof(char))))
		return (clean_exit(buffer, -1));
	**line = '\0';
	if (eolp(fdbuffer[fd], 0) != -1)
	{
		if (!(*line = ultstrcat(*line, fdbuffer[fd])))
			return (clean_exit(buffer, -1));
		delol(fdbuffer[fd], 0);
		return (1);
	}
	return (0);
}

int	readtnl(int fd, char **buffer, char **line)
{
	int read_ret;

	while ((read_ret = read(fd, *buffer, BUFFER_SIZE)) > 0
			&& eolp(*buffer, 0) == -1)
	{
		(*buffer)[read_ret] = 0;
		if (!(*line = ultstrcat(*line, *buffer)))
			return (-1);
		delol(*buffer, 1);
	}
	return (read_ret);
}

int	get_next_line(int fd, char **line)
{
	char				*buffer;
	static char			fdbuffer[_SC_OPEN_MAX][BUFFER_SIZE + 1];
	int					read_ret;

	if ((read_ret = prep_gnl(fd, line, &buffer, fdbuffer)) == -1)
		return (-1);
	else if (read_ret)
		return (clean_exit(&buffer, 1));
	else if (eolp(fdbuffer[fd], 0) == -1 && *(fdbuffer[fd]))
	{
		if (!(*line = ultstrcat(*line, fdbuffer[fd])))
			return (clean_exit(&buffer, -1));
		delol(fdbuffer[fd], 1);
	}
	delol(buffer, 1);
	if ((read_ret = readtnl(fd, &buffer, line)) == -1)
		return (clean_exit(&buffer, -1));
	if (eolp(buffer, 0) != -1)
	{
		buffer[read_ret] = 0;
		if (!(*line = ultstrcat(*line, buffer)))
			return (clean_exit(&buffer, -1));
		fill_tempbuffer(fdbuffer[fd], buffer, eolp(buffer, 0));
	}
	return (clean_exit(&buffer, (read_ret > 0) ? 1 : read_ret));
}
