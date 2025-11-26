#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "get_next_line_bonus.h"

typedef struct s_fd_ctx
{
	int			fd;
	const char	*label;
	bool		is_stdin;
	size_t		line_no;
}	t_fd_ctx;

static void	print_usage(const char *prog)
{
	fprintf(stderr,
		"使い方: %s <file> [file ...]\n"
		"  '-' を指定すると標準入力を対象にします。\n"
		"  全FDをラウンドロビンで読み出し、bonusの同時FD対応を確認できます。\n",
		prog);
}

static void	close_ctx(t_fd_ctx *ctx)
{
	if (ctx->fd >= 0 && !ctx->is_stdin)
		close(ctx->fd);
	ctx->fd = -1;
}

static bool	init_ctx(t_fd_ctx *ctx, const char *path)
{
	if (strcmp(path, "-") == 0)
	{
		ctx->fd = STDIN_FILENO;
		ctx->label = "stdin";
		ctx->is_stdin = true;
		ctx->line_no = 0;
		return (true);
	}
	ctx->fd = open(path, O_RDONLY);
	if (ctx->fd < 0)
	{
		fprintf(stderr, "open失敗: %s: %s\n", path, strerror(errno));
		return (false);
	}
	ctx->label = path;
	ctx->is_stdin = false;
	ctx->line_no = 0;
	return (true);
}

int	main(int argc, char **argv)
{
	t_fd_ctx	*ctxs;
	int			total;
	int			valid;
	int			i;
	int			finished;
	char		*line;

	if (argc < 2)
	{
		print_usage(argv[0]);
		return (1);
	}
	total = argc - 1;
	ctxs = malloc(sizeof(*ctxs) * total);
	if (ctxs == NULL)
	{
		perror("malloc");
		return (1);
	}
	valid = 0;
	i = 1;
	while (i < argc)
	{
		if (init_ctx(&ctxs[valid], argv[i]))
			++valid;
		++i;
	}
	if (valid == 0)
	{
		free(ctxs);
		return (1);
	}
	finished = 0;
	while (finished < valid)
	{
		i = 0;
		while (i < valid)
		{
			if (ctxs[i].fd >= 0)
			{
				line = get_next_line(ctxs[i].fd);
				if (line == NULL)
				{
					close_ctx(&ctxs[i]);
					++finished;
				}
				else
				{
					printf("[BONUS][%s][%04zu] %s",
						ctxs[i].label, ctxs[i].line_no, line);
					if (line[0] == '\0' || line[strlen(line) - 1] != '\n')
						printf("\n");
					++ctxs[i].line_no;
					free(line);
				}
			}
			++i;
		}
	}
	free(ctxs);
	return (0);
}
