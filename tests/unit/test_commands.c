/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:46:36 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:46:38 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/commands/commands.h"
#include "capture.h"

#include <stdio.h>
#include <stdlib.h>

/*
* capture_begin_clean() flushes stdio first: capture_end() calls fflush(NULL),
* so buffered Unity PASS/FAIL lines would otherwise leak into the captured
* string and break the exact-match version() assertion.
*/
static int	capture_begin_clean(int fd, t_capture *cap)
{
	fflush(NULL);
	return (capture_begin(fd, cap));
}

static void	capture_both_begin(t_capture *cout, t_capture *cerr)
{
	capture_begin_clean(1, cout);
	capture_begin_clean(2, cerr);
}

static void	capture_both_end(t_capture *cout, t_capture *cerr,
				char **out_str, char **err_str)
{
	*err_str = capture_end(cerr);
	*out_str = capture_end(cout);
}

static void	test_version_exact_output(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, version());
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("codexion v0.1\n", out);
	free(out);
}

static void	test_process_command_dash_v(void)
{
	char		*argv[] = {"prog", "-v"};
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, process_command(argv));
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "codexion v0.1"));
	free(out);
}

static void	test_process_command_dash_dash_version(void)
{
	char		*argv[] = {"prog", "--version"};
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, process_command(argv));
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "codexion v0.1"));
	free(out);
}

static void	test_process_command_dash_h(void)
{
	char		*argv[] = {"prog", "-h"};
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, process_command(argv));
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "Usage"));
	TEST_ASSERT_TRUE(str_contains(out, "Commands:"));
	free(out);
}

static void	test_process_command_dash_dash_help(void)
{
	char		*argv[] = {"prog", "--help"};
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, process_command(argv));
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "Usage"));
	TEST_ASSERT_TRUE(str_contains(out, "Commands:"));
	free(out);
}

/*
* Unknown commands go through print_error(..., true): the error line goes to
* stderr, the usage/help dump (which contains "Usage") goes to stdout.
*/
static void	test_process_command_unknown(void)
{
	char		*argv[] = {"prog", "-z"};
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, process_command(argv));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	TEST_ASSERT_TRUE(str_contains(err_str, "UNKNOWN COMMAND"));
	TEST_ASSERT_TRUE(str_contains(out_str, "Usage"));
	free(out_str);
	free(err_str);
}

static void	test_help_output(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, help());
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "codexion Usage"));
	TEST_ASSERT_TRUE(str_contains(out, "Arguments:"));
	TEST_ASSERT_TRUE(str_contains(out, "number_of_coders"));
	TEST_ASSERT_TRUE(str_contains(out, "time_to_burnout"));
	/* source names the token "numbers_of_compiles_required" (plural) */
	TEST_ASSERT_TRUE(str_contains(out, "numbers_of_compiles_required"));
	TEST_ASSERT_TRUE(str_contains(out, "dongle_cooldown"));
	TEST_ASSERT_TRUE(str_contains(out, "scheduler"));
	free(out);
}

void	run_commands_tests(void)
{
	RUN_TEST(test_version_exact_output);
	RUN_TEST(test_process_command_dash_v);
	RUN_TEST(test_process_command_dash_dash_version);
	RUN_TEST(test_process_command_dash_h);
	RUN_TEST(test_process_command_dash_dash_help);
	RUN_TEST(test_process_command_unknown);
	RUN_TEST(test_help_output);
}