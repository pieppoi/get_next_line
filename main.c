#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(GNL_TEST_BONUS) || defined(GNL_BONUS) || defined(USE_GNL_BONUS)
# include "get_next_line_bonus.h"
#else
# include "get_next_line.h"
#endif

int	main(int argc, char **argv)
{
	bool	round_robin;
	int		start;
	int		count;
	int		i;
	int		finished;
	int		*fds;
	const char **labels;
	bool	*is_stdin;
	size_t	*line_no;
	char	*line;

	if (argc < 2)
	{
		fprintf(stderr, "使い方: %s [-r] <file> [file ...]\n", argv[0]);
		fprintf(stderr, "  '-' で標準入力を対象にできます。-r でラウンドロビン。\n");
		return (1);
	}
	start = 1;
	round_robin = false;
	if (strcmp(argv[1], "-r") == 0)
	{
		round_robin = true;
		start = 2;
	}
	if (start >= argc)
	{
		fprintf(stderr, "ファイル名を指定してください。\n");
		return (1);
	}
	count = argc - start;
	fds = malloc(sizeof(int) * count);
	labels = malloc(sizeof(char *) * count);
	is_stdin = malloc(sizeof(bool) * count);
	line_no = calloc(count, sizeof(size_t));
	if (!fds || !labels || !is_stdin || !line_no)
	{
		perror("malloc");
		return (1);
	}
	for (i = 0; i < count; ++i)
	{
		const char	*path = argv[start + i];

		if (strcmp(path, "-") == 0)
		{
			fds[i] = STDIN_FILENO;
			is_stdin[i] = true;
			labels[i] = "stdin";
		}
		else
		{
			fds[i] = open(path, O_RDONLY);
			if (fds[i] < 0)
				fprintf(stderr, "open失敗: %s: %s\n", path, strerror(errno));
			is_stdin[i] = false;
			labels[i] = path;
		}
	}
	if (!round_robin)
	{
		for (i = 0; i < count; ++i)
		{
			if (fds[i] < 0)
				continue;
			printf("==== %s ====\n", labels[i]);
			while ((line = get_next_line(fds[i])) != NULL)
			{
				printf("[SEQ][%s][%04zu] %s", labels[i], line_no[i], line);
				if (line[0] == '\0' || line[strlen(line) - 1] != '\n')
					printf("\n");
				++line_no[i];
				free(line);
			}
			if (!is_stdin[i])
				close(fds[i]);
		}
	}
	else
	{
		finished = 0;
		while (finished < count)
		{
			for (i = 0; i < count; ++i)
			{
				if (fds[i] < 0)
					continue;
				line = get_next_line(fds[i]);
				if (!line)
				{
					if (!is_stdin[i])
						close(fds[i]);
					fds[i] = -1;
					++finished;
					continue;
				}
				printf("[RR ][%s][%04zu] %s", labels[i], line_no[i], line);
				if (line[0] == '\0' || line[strlen(line) - 1] != '\n')
					printf("\n");
				++line_no[i];
				free(line);
			}
		}
	}
	free(fds);
	free(labels);
	free(is_stdin);
	free(line_no);
	return (0);
}
