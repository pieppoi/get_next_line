/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:28:54 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/25 05:57:12 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <limits.h>

static t_list	*get_remove_node(t_list **lst, int fd, int is_rm)
{
	t_list	*tm;
	t_list	*prv;

	tm = *lst;
	prv = NULL;
	while (tm && tm->fd != fd)
	{
		prv = tm;
		tm = tm->next;
	}
	if (tm == NULL && is_rm == 0)
	{
		tm = malloc(sizeof(t_list));
		if (!tm)
			return (NULL);
		return (tm->fd = fd, tm->cache = NULL, tm->next = *lst, *lst = tm, tm);
	}
	if (tm && is_rm && prv == NULL)
		return (*lst = tm->next, free(tm->cache), free(tm), NULL);
	if (tm && is_rm && prv != NULL)
		return (prv->next = tm->next, free(tm->cache), free(tm), NULL);
	return (tm);
}

static char	*get_line(char *cache)
{
	char	*line;
	size_t	total;
	size_t	i;

	if (cache == NULL || cache[0] == '\0')
		return (NULL);
	i = 0;
	while (cache[i] && cache[i] != '\n')
		i++;
	total = i + 1 + (cache[i] == '\n');
	line = malloc(total);
	if (!line)
		return (NULL);
	ft_strlcpy(line, cache, total);
	return (line);
}

static char	*read_to_cache(int fd, char *cache)
{
	char	*buff;
	ssize_t	bytes_read;

	buff = malloc(((size_t)BUFFER_SIZE) + 1);
	if (!buff)
		return (free(cache), NULL);
	bytes_read = 1;
	while (ft_strchr(cache, '\n') == NULL && bytes_read > 0)
	{
		bytes_read = read(fd, buff, BUFFER_SIZE);
		if (bytes_read < 0)
			return (free(buff), free(cache), NULL);
		if (bytes_read == 0)
			return (free(buff), cache);
		buff[bytes_read] = '\0';
		if (cache == NULL)
			cache = ft_strdup(buff);
		else
			cache = ft_strjoin(cache, buff);
		if (cache == NULL)
			return (free(buff), NULL);
	}
	free(buff);
	return (cache);
}

static char	*remove_line_cache(char *cache)
{
	char	*after_line;
	int		i;

	if (cache == NULL)
		return (NULL);
	i = 0;
	while (cache[i] && cache[i] != '\n')
		i++;
	if (cache[i] == '\0')
	{
		free(cache);
		return (NULL);
	}
	after_line = ft_strdup(cache + i + 1);
	free(cache);
	return (after_line);
}

char	*get_next_line(int fd)
{
	char			*line;
	static t_list	*lst;
	t_list			*node;

	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SSIZE_MAX)
		return (get_remove_node(&lst, fd, 1), NULL);
	node = get_remove_node(&lst, fd, 0);
	if (!node)
		return (NULL);
	node->cache = read_to_cache(fd, node->cache);
	if (node->cache == NULL)
	{
		get_remove_node(&lst, fd, 1);
		return (NULL);
	}
	line = get_line(node->cache);
	if (!line)
	{
		get_remove_node(&lst, fd, 1);
		return (NULL);
	}
	node->cache = remove_line_cache(node->cache);
	if (!node->cache)
		get_remove_node(&lst, fd, 1);
	return (line);
}
