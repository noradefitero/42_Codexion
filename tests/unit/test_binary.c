/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_binary.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:43:53 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 22:18:25 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * tests that fork/exec the compiled codexion binary (CODEXION_BIN) and assert
 * exit codes, stdout/stderr and simulation runs.
*/

#include "../../Unity/src/unity.h"
#include "../../src/utils/time.h"
#include "capture.h"

#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef CODEXION_BIN
# define CODEXION_BIN "./codexion"
#endif

# define RUN_DEADLINE_MS 45000

typedef struct s_run
{
	int			status;
	int			signaled;
	int			exit_code;
	char		*out;
	char		*err;
	long long	elapsed_ms;
}	t_run;

typedef struct s_buf
{
	char	*data;
	size_t	len;
	size_t	cap;
}	t_buf;

/* Unity links one setUp/tearDown pair per binary; this suite keeps its own
 * and has no shared fixture, so both stay empty. */
void	setUp(void)
{
}

void	tearDown(void)
{
}

static void	buf_append(t_buf *b, const char *p, size_t n)
{
	size_t	need;

	need = b->len + n + 1;
	if (need > b->cap)
	{
		size_t	new_cap = b->cap ? b->cap : 4096;

		while (new_cap < need)
			new_cap *= 2;
		b->data = (char *)realloc(b->data, new_cap);
		b->cap = new_cap;
	}
	memcpy(b->data + b->len, p, n);
	b->len += n;
	b->data[b->len] = '\0';
}

/* Non-blocking drain: reads whatever is available, stops on EAGAIN/EOF. */
static void	drain_fd(int fd, t_buf *b)
{
	char	tmp[4096];
	ssize_t	n;

	while ((n = read(fd, tmp, sizeof(tmp))) > 0)
		buf_append(b, tmp, (size_t)n);
}

static void	close_both(int p[2])
{
	if (p[0] >= 0)
		close(p[0]);
	if (p[1] >= 0)
		close(p[1]);
	p[0] = -1;
	p[1] = -1;
}

static void	run_codexion(t_run *r, char *argv[])
{
	int				out_pipe[2] = {-1, -1};
	int				err_pipe[2] = {-1, -1};
	pid_t			pid;
	int				wstatus = 0;
	t_buf			out = {0};
	t_buf			err = {0};
	t_ms			start;
	struct pollfd	pfds[2];

	memset(r, 0, sizeof(*r));
	r->exit_code = -1;
	if (pipe(out_pipe) != 0 || pipe(err_pipe) != 0)
	{
		close_both(out_pipe);
		close_both(err_pipe);
		r->out = strdup("");
		r->err = strdup("");
		return ;
	}
	pid = fork();
	if (pid < 0)
	{
		close_both(out_pipe);
		close_both(err_pipe);
		r->out = strdup("");
		r->err = strdup("");
		return ;
	}
	if (pid == 0)
	{
		dup2(out_pipe[1], STDOUT_FILENO);
		dup2(err_pipe[1], STDERR_FILENO);
		close_both(out_pipe);
		close_both(err_pipe);
		execv(CODEXION_BIN, argv);
		_exit(127);
	}

	/* parent: keep read ends, drop write ends */
	close(out_pipe[1]);
	close(err_pipe[1]);
	out_pipe[1] = -1;
	err_pipe[1] = -1;
	fcntl(out_pipe[0], F_SETFL, fcntl(out_pipe[0], F_GETFL, 0) | O_NONBLOCK);
	fcntl(err_pipe[0], F_SETFL, fcntl(err_pipe[0], F_GETFL, 0) | O_NONBLOCK);

	pfds[0].fd = out_pipe[0];
	pfds[0].events = POLLIN;
	pfds[1].fd = err_pipe[0];
	pfds[1].events = POLLIN;

	start = get_time();
	for (;;)
	{
		pid_t	w = waitpid(pid, &wstatus, WNOHANG);

		if (w == pid || w < 0)
			break ;
		drain_fd(out_pipe[0], &out);
		drain_fd(err_pipe[0], &err);
		if (get_time() - start > RUN_DEADLINE_MS)
		{
			kill(pid, SIGKILL);
			r->signaled = 1;
			waitpid(pid, &wstatus, 0);
			break ;
		}
		poll(pfds, 2, 10);
	}

	/* child's write ends are closed: read the rest to EOF */
	drain_fd(out_pipe[0], &out);
	drain_fd(err_pipe[0], &err);
	close(out_pipe[0]);
	close(err_pipe[0]);

	r->status = wstatus;
	if (WIFSIGNALED(wstatus))
		r->signaled = 1;
	else if (WIFEXITED(wstatus))
		r->exit_code = WEXITSTATUS(wstatus);
	r->elapsed_ms = get_time() - start;
	r->out = out.data ? out.data : strdup("");
	r->err = err.data ? err.data : strdup("");
}

static void	free_run(t_run *r)
{
	free(r->out);
	free(r->err);
	r->out = NULL;
	r->err = NULL;
}

static int	count_in(const char *s, const char *needle)
{
	const char	*p;
	size_t		n;
	int			count;

	if (!s || !needle || !*needle)
		return (0);
	p = s;
	n = strlen(needle);
	count = 0;
	while ((p = strstr(p, needle)) != NULL)
	{
		count++;
		p += n;
	}
	return (count);
}

/* 1. no arguments → help(), exit 0, "Usage" on stdout */
static void	test_binary_no_args(void)
{
	t_run	r;
	char	*argv[] = { CODEXION_BIN, NULL };

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "Usage"));
	free_run(&r);
}

/* 2. -h / --help → help(), exit 0 */
static void	test_binary_help(void)
{
	t_run	r;
	char	*argv_h[] = { CODEXION_BIN, "-h", NULL };
	char	*argv_hh[] = { CODEXION_BIN, "--help", NULL };

	run_codexion(&r, argv_h);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "Usage"));
	TEST_ASSERT_TRUE(str_contains(r.out, "Commands:"));
	free_run(&r);

	run_codexion(&r, argv_hh);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "Usage"));
	TEST_ASSERT_TRUE(str_contains(r.out, "Commands:"));
	free_run(&r);
}

/* 3. -v / --version → exact "codexion v0.1\n" on stdout, exit 0 */
static void	test_binary_version(void)
{
	t_run	r;
	char	*argv_v[] = { CODEXION_BIN, "-v", NULL };
	char	*argv_vv[] = { CODEXION_BIN, "--version", NULL };

	run_codexion(&r, argv_v);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_STRING("codexion v0.1\n", r.out);
	free_run(&r);

	run_codexion(&r, argv_vv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_STRING("codexion v0.1\n", r.out);
	free_run(&r);
}

/* 4. unknown command → print_error → exit 255 (0xFF) */
static void	test_binary_unknown_command(void)
{
	t_run	r;
	char	*argv[] = { CODEXION_BIN, "-z", NULL };

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(255, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "UNKNOWN COMMAND"));
	TEST_ASSERT_TRUE(str_contains(r.out, "Usage"));
	free_run(&r);
}

/* 5. extra args after a valid flag are ignored (argv[1] only) */
static void	test_binary_flag_extra_args(void)
{
	t_run	r;
	char	*argv[] = { CODEXION_BIN, "-h", "junk", NULL };

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "Usage"));
	free_run(&r);
}

/* 6. argc must be 9 (prog + 8): 8 and 10 both rejected */
static void	test_binary_wrong_argc(void)
{
	t_run	r;
	char	*argv8[] = {
		CODEXION_BIN, "1", "2", "3", "4", "5", "6", "7", NULL
	};
	char	*argv10[] = {
		CODEXION_BIN, "1", "2", "3", "4", "5", "6", "7", "fifo", "extra", NULL
	};

	run_codexion(&r, argv8);
	TEST_ASSERT_EQUAL_INT(1, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "INVALID SINTAX"));
	free_run(&r);

	run_codexion(&r, argv10);
	TEST_ASSERT_EQUAL_INT(1, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "INVALID SINTAX"));
	free_run(&r);
}

/* 7. non-digit (and sign) characters in numeric fields → INVALID NUMBER */
static void	test_binary_non_digit(void)
{
	t_run	r;
	char	*argv_x[] = {
		CODEXION_BIN, "1", "x", "3", "4", "5", "6", "7", "fifo", NULL
	};
	char	*argv_neg[] = {
		CODEXION_BIN, "1", "-2", "3", "4", "5", "6", "7", "fifo", NULL
	};

	run_codexion(&r, argv_x);
	TEST_ASSERT_EQUAL_INT(1, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "INVALID NUMBER"));
	free_run(&r);

	run_codexion(&r, argv_neg);
	TEST_ASSERT_EQUAL_INT(1, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "INVALID NUMBER"));
	free_run(&r);
}

/* 8. unknown scheduler name → INVALID SCHEDULER */
static void	test_binary_bad_scheduler(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "1", "2", "3", "4", "5", "6", "7", "lifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(1, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "INVALID SCHEDULER"));
	free_run(&r);
}

/* 9. number_of_coders == 0 → silent success, empty stdout */
static void	test_binary_zero_coders(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "0", "100", "10", "10", "10", "3", "0", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_STRING("", r.out);
	TEST_ASSERT_EQUAL_STRING("", r.err);
	free_run(&r);
}

/* 10. number_of_compiles_required == 0 → silent success, empty stdout */
static void	test_binary_zero_compiles(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "2", "100", "10", "10", "10", "0", "0", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_STRING("", r.out);
	TEST_ASSERT_EQUAL_STRING("", r.err);
	free_run(&r);
}

/* 11. N=1: the lone coder takes its dongle, waits and burns out */
static void	test_binary_n1_burn(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "1", "150", "30", "20", "20", "2", "0", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "has taken a dongle"));
	TEST_ASSERT_TRUE(str_contains(r.out, "burned out"));
	TEST_ASSERT_TRUE(r.elapsed_ms < 10000);
	free_run(&r);
}

/* 12. N=2 feasible fifo: exactly 4 "is compiling" logs, no burnout */
static void	test_binary_n2_fifo(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "2", "800", "20", "10", "10", "2", "0", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_INT(4, count_in(r.out, "is compiling"));
	TEST_ASSERT_FALSE(str_contains(r.out, "burned out"));
	TEST_ASSERT_TRUE(r.elapsed_ms < 10000);
	free_run(&r);
}

/* 13. N=2 feasible edf: same guarantees under the EDF scheduler */
static void	test_binary_n2_edf(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "2", "800", "20", "10", "10", "2", "0", "edf", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_EQUAL_INT(4, count_in(r.out, "is compiling"));
	TEST_ASSERT_FALSE(str_contains(r.out, "burned out"));
	TEST_ASSERT_TRUE(r.elapsed_ms < 10000);
	free_run(&r);
}

/* 14. README burn scenario: compile(34) > burnout(20) → someone burns out */
static void	test_binary_readme_burn(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "2", "20", "34", "10", "34", "20", "50", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.out, "burned out"));
	TEST_ASSERT_TRUE(r.elapsed_ms < 10000);
	free_run(&r);
}

/* 15. subject scenario: 5 coders in a ring finish all 5 compiles without
 * burning out within the 800ms budget (runs a few seconds). */
static void	test_binary_subject_no_burn(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "5", "800", "200", "200", "200", "5", "10", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(0, r.exit_code);
	TEST_ASSERT_FALSE(str_contains(r.out, "burned out"));
	TEST_ASSERT_TRUE(r.elapsed_ms < 30000);
	free_run(&r);
}

/* 16. absurd coder count: atoi wraps LONG_MAX to -1, hub__create_usbs casts
 * it to size_t 2^64-1 and ft_calloc's overflow guard (nmemb > SIZE_MAX/8)
 * returns NULL immediately, so hub__init fails and main returns -1 → exit 255.
 * NOTE: INT_MAX (2147483647) is NOT used: on macOS calloc(INT_MAX, 8) ≈ 17GB
 * succeeds through VM overcommit, and the hub then hangs creating 2^31 dongles
 * until the harness's 45s deadline SIGKILLs it. */
static void	test_binary_absurd_coders(void)
{
	t_run	r;
	char	*argv[] = {
		CODEXION_BIN, "9223372036854775807",
		"1", "1", "1", "1", "1", "1", "fifo", NULL
	};

	run_codexion(&r, argv);
	TEST_ASSERT_EQUAL_INT(255, r.exit_code);
	TEST_ASSERT_TRUE(str_contains(r.err, "FAILED ALLOCATING"));
	free_run(&r);
}

void	run_binary_tests(void)
{
	RUN_TEST(test_binary_no_args);
	RUN_TEST(test_binary_help);
	RUN_TEST(test_binary_version);
	RUN_TEST(test_binary_unknown_command);
	RUN_TEST(test_binary_flag_extra_args);
	RUN_TEST(test_binary_wrong_argc);
	RUN_TEST(test_binary_non_digit);
	RUN_TEST(test_binary_bad_scheduler);
	RUN_TEST(test_binary_zero_coders);
	RUN_TEST(test_binary_zero_compiles);
	RUN_TEST(test_binary_n1_burn);
	RUN_TEST(test_binary_n2_fifo);
	RUN_TEST(test_binary_n2_edf);
	RUN_TEST(test_binary_readme_burn);
	RUN_TEST(test_binary_subject_no_burn);
	RUN_TEST(test_binary_absurd_coders);
}