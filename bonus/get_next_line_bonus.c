/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:28:54 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 03:53:03 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

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

	if (!save)
		return (NULL);
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

	if (!save)
		return (NULL);
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
	static t_list	*head = NULL;
	t_list			*node;
	char			*str;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	node = find_fd_node(&head, fd);
	if (!node)
		return (NULL);
	node->save = next_line(fd, node->save);
	if (!node->save)
	{
		remove_fd_node(&head, fd);
		return (NULL);
	}
	str = find_nextline(node->save);
	if (!str)
	{
		remove_fd_node(&head, fd);
		return (NULL);
	}
	node->save = next_save(node->save);
	if (!node->save)
		remove_fd_node(&head, fd);
	return (str);
}
