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

typedef struct s_fd_slot
{
	int			fd;
	const char	*label;
	bool		is_stdin;
	size_t		line_no;
}	t_fd_slot;

static void	print_usage(const char *prog_name)
{
	fprintf(stderr,
		"使い方: %s [-r|--round-robin] <file> [file ...]\n"
		"  '-' を指定すると標準入力を対象にします。\n"
		"  -r/--round-robin は複数FDを交互に読むbonusテスト用モードです。\n",
		prog_name);
}

static bool	is_round_robin_flag(const char *arg)
{
	return (strcmp(arg, "-r") == 0 || strcmp(arg, "--round-robin") == 0);
}

static bool	prepare_slot(t_fd_slot *slot, const char *path)
{
	if (strcmp(path, "-") == 0)
	{
		slot->fd = STDIN_FILENO;
		slot->label = "stdin";
		slot->is_stdin = true;
		slot->line_no = 0;
		return (true);
	}
	slot->fd = open(path, O_RDONLY);
	if (slot->fd < 0)
	{
		fprintf(stderr, "open失敗: %s: %s\n", path, strerror(errno));
		return (false);
	}
	slot->label = path;
	slot->is_stdin = false;
	slot->line_no = 0;
	return (true);
}

static void	close_slot(t_fd_slot *slot)
{
	if (slot->fd >= 0 && !slot->is_stdin)
		close(slot->fd);
	slot->fd = -1;
}

static void	emit_line(const char *mode, t_fd_slot *slot, char *line)
{
	size_t	len;

	len = strlen(line);
	printf("[%s][%s][%04zu] %s", mode, slot->label, slot->line_no, line);
	if (len == 0 || line[len - 1] != '\n')
		printf("\n");
	slot->line_no++;
}

static void	run_sequential(t_fd_slot *slots, size_t count)
{
	size_t	i;
	char	*line;

	for (i = 0; i < count; ++i)
	{
		printf("==== %s (SEQ) ====\n", slots[i].label);
		while (slots[i].fd >= 0 && (line = get_next_line(slots[i].fd)) != NULL)
		{
			emit_line("SEQ", &slots[i], line);
			free(line);
		}
		close_slot(&slots[i]);
	}
}

static void	run_round_robin(t_fd_slot *slots, size_t count)
{
	size_t	finished;
	size_t	i;
	char	*line;

	finished = 0;
	while (finished < count)
	{
		i = 0;
		while (i < count)
		{
			if (slots[i].fd >= 0)
			{
				line = get_next_line(slots[i].fd);
				if (line == NULL)
				{
					close_slot(&slots[i]);
					++finished;
				}
				else
				{
					emit_line("RR", &slots[i], line);
					free(line);
				}
			}
			++i;
		}
	}
}

int	main(int argc, char **argv)
{
	bool		round_robin;
	int			argi;
	size_t		targets;
	size_t		valid;
	t_fd_slot	*slots;

	if (argc < 2)
	{
		print_usage(argv[0]);
		return (1);
	}
	argi = 1;
	round_robin = false;
	if (argi < argc && is_round_robin_flag(argv[argi]))
	{
		round_robin = true;
		++argi;
	}
	if (argi >= argc)
	{
		print_usage(argv[0]);
		return (1);
	}
	targets = argc - argi;
	slots = calloc(targets, sizeof(*slots));
	if (slots == NULL)
	{
		perror("calloc");
		return (1);
	}
	valid = 0;
	while (argi < argc)
	{
		if (prepare_slot(&slots[valid], argv[argi]))
			++valid;
		++argi;
	}
	if (valid == 0)
	{
		free(slots);
		return (1);
	}
	if (round_robin)
		run_round_robin(slots, valid);
	else
		run_sequential(slots, valid);
	free(slots);
	return (0);
}
