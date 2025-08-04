/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:28:54 by mkazuhik          #+#    #+#             */
/*   Updated: 2024/07/30 15:24:55 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*mem_free(char *free1, char *free2)
{
	free (free1);
	free (free2);
	return (NULL);
}

char	*next_line(int fd, char *save)
{
	char	*tmp;
	char	*buf;
	ssize_t	readlen;

	tmp = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!tmp)
	{
		free(save);
		return (NULL);
	}
	readlen = 1;
	while (!ft_strchr(save, '\n') && readlen != 0)
	{
		readlen = read(fd, tmp, BUFFER_SIZE);
		if (readlen == -1 || (readlen == 0 && save[0] == '\0'))
			return (mem_free(tmp, save));
		tmp[readlen] = '\0';
		buf = ft_strjoin(save, tmp);
		if (!buf)
			return (mem_free(tmp, save));
		free(save);
		save = buf;
	}
	free(tmp);
	return (save);
}

char	*find_nextline(char *save)
{
	int		i;
	char	*str;

	i = 0;
	while (save[i] != '\0' && save[i] != '\n')
		i++;
	str = malloc((i + 2) * sizeof(char));
	if (!str)
		return (NULL);
	i = 0;
	while (save[i])
	{
		str[i] = save[i];
		if (str[i] == '\n')
		{
			i++;
			break ;
		}
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*next_save(char *save)
{
	size_t	i;
	char	*tmp;

	i = 0;
	while (save[i] != '\0' && save[i] != '\n')
		i++;
	if (!save[i])
	{
		free(save);
		return (NULL);
	}
	i++;
	tmp = ft_strdup(&save[i]);
	free(save);
	if (!tmp)
		return (NULL);
	save = tmp;
	return (save);
}

char	*get_next_line(int fd)
{
	static char	*save;
	char		*str;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!save)
	{
		save = ft_strdup("");
		if (!save)
			return (NULL);
	}
	save = next_line(fd, save);
	if (!save)
		return (NULL);
	str = find_nextline(save);
	if (!str)
	{
		free (save);
		return (NULL);
	}
	save = next_save(save);
	return (str);
}
