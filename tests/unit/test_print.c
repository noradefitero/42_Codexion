/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_print.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:47:40 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:42 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/io/print.h"
#include "capture.h"

#include <stdio.h>
#include <stdlib.h>

/*
* print_error() writes the "[1] ERROR: ..." line to stderr, but its help
* output (print_help == true) goes to stdout through help(). Capture both
* fds at once and assert on each stream independently.
*
* capture_begin_clean() flushes stdio first: capture_end() calls fflush(NULL),
* so buffered Unity PASS/FAIL lines would otherwise leak into the captured
* string and break the exact-match assertions below.
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

static void	test_print_error_without_help(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(2, &cap);
	TEST_ASSERT_EQUAL_INT(-1, print_error("TESTMSG", false));
	out = capture_end(&cap);
	TEST_ASSERT_TRUE(str_contains(out, "[1] ERROR:"));
	TEST_ASSERT_TRUE(str_contains(out, "TESTMSG"));
	free(out);
}

static void	test_print_error_with_help(void)
{
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, print_error("TESTMSG", true));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	/* (help() output actually lands on stdout, not stderr) */
	TEST_ASSERT_TRUE(str_contains(err_str, "[1] ERROR:"));
	TEST_ASSERT_TRUE(str_contains(err_str, "TESTMSG"));
	TEST_ASSERT_TRUE(str_contains(out_str, "Usage"));
	free(out_str);
	free(err_str);
}

static void	test_log_state_taken_dongle(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(1, 10, TAKEN_DONGLE);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("10 1 has taken a dongle\n", out);
	free(out);
}

static void	test_log_state_compiling(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(3, 42, COMPILING);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("42 3 is compiling\n", out);
	free(out);
}

static void	test_log_state_debugging(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(2, 20, DEBUGGING);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("20 2 is debugging\n", out);
	free(out);
}

static void	test_log_state_refactoring(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(3, 30, REFACTORING);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("30 3 is refactoring\n", out);
	free(out);
}

/* LAST BUT ONE: after this runs, the process-global `burned` flag is set and
 * no log_state() output can ever be observed again. */
static void	test_log_state_burned(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(4, 40, BURNED);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("40 4 burned out\n", out);
	free(out);
}

/* MUST BE LAST: the BURNED test above latched the global `burned` flag, so
 * every subsequent log_state() call is silently suppressed. */
static void	test_log_state_suppressed_after_burned(void)
{
	t_capture	cap;
	char		*out;

	capture_begin_clean(1, &cap);
	log_state(1, 99, COMPILING);
	out = capture_end(&cap);
	TEST_ASSERT_EQUAL_STRING("", out);
	free(out);
}

void	run_print_tests(void)
{
	RUN_TEST(test_print_error_without_help);
	RUN_TEST(test_print_error_with_help);
	RUN_TEST(test_log_state_taken_dongle);
	RUN_TEST(test_log_state_compiling);
	RUN_TEST(test_log_state_debugging);
	RUN_TEST(test_log_state_refactoring);
	/* order matters from here on — see header comment */
	RUN_TEST(test_log_state_burned);
	RUN_TEST(test_log_state_suppressed_after_burned);
}