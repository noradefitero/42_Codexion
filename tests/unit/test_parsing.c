/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:47:31 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:33 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/io/parsing_params.h"
#include "capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void	test_valid_fifo_args(void)
{
	char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "fifo"};
	t_config	cfg;

	memset(&cfg, 0, sizeof(cfg));
	TEST_ASSERT_EQUAL_INT(0, parse_arguments(&cfg, 9, argv));
	TEST_ASSERT_EQUAL_INT(3, cfg.number_of_coders);
	TEST_ASSERT_EQUAL_INT(800, cfg.time_to_burnout);
	TEST_ASSERT_EQUAL_INT(200, cfg.time_to_compile);
	TEST_ASSERT_EQUAL_INT(100, cfg.time_to_debug);
	TEST_ASSERT_EQUAL_INT(50, cfg.time_to_refactor);
	TEST_ASSERT_EQUAL_INT(5, cfg.number_of_compiles_required);
	TEST_ASSERT_EQUAL_INT(10, cfg.dongle_cooldown);
	TEST_ASSERT_EQUAL_INT(FIFO, cfg.scheduler);
}

static void	test_valid_edf_scheduler(void)
{
	char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "edf"};
	t_config	cfg;

	memset(&cfg, 0, sizeof(cfg));
	TEST_ASSERT_EQUAL_INT(0, parse_arguments(&cfg, 9, argv));
	TEST_ASSERT_EQUAL_INT(EDF, cfg.scheduler);
}

static void	test_all_zero_arguments(void)
{
	char		*argv[] = {"prog", "0", "0", "0", "0", "0", "0", "0", "fifo"};
	t_config	cfg;

	memset(&cfg, 0, sizeof(cfg));
	TEST_ASSERT_EQUAL_INT(0, parse_arguments(&cfg, 9, argv));
	TEST_ASSERT_EQUAL_INT(0, cfg.number_of_coders);
	TEST_ASSERT_EQUAL_INT(0, cfg.time_to_burnout);
	TEST_ASSERT_EQUAL_INT(0, cfg.time_to_compile);
	TEST_ASSERT_EQUAL_INT(0, cfg.time_to_debug);
	TEST_ASSERT_EQUAL_INT(0, cfg.time_to_refactor);
	TEST_ASSERT_EQUAL_INT(0, cfg.number_of_compiles_required);
	TEST_ASSERT_EQUAL_INT(0, cfg.dongle_cooldown);
	TEST_ASSERT_EQUAL_INT(FIFO, cfg.scheduler);
}

static void	test_wrong_argc_too_few(void)
{
	char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10"};
	t_config	cfg;
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	memset(&cfg, 0, sizeof(cfg));
	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 8, argv));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	TEST_ASSERT_TRUE(str_contains(err_str, "INVALID SINTAX"));
	free(out_str);
	free(err_str);
}

static void	test_wrong_argc_too_many(void)
{
	char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "fifo", "extra"};
	t_config	cfg;
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	memset(&cfg, 0, sizeof(cfg));
	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 10, argv));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	TEST_ASSERT_TRUE(str_contains(err_str, "INVALID SINTAX"));
	free(out_str);
	free(err_str);
}

/*
* Every numeric position (argv[1..7]) is probed with a battery of non-digit
* strings: non-digit char, negative sign, plus sign, leading/trailing space,
* alphanumeric mix and decimal point. All must fail with INVALID NUMBER.
*
* The empty string "" is deliberately NOT here: ft_strisdigit() vacuously
* accepts it (a zero-length string contains no non-digit character), so
* parse_arguments() succeeds and stores atoi("") == 0. See
* test_empty_numeric_string below.
*/
static void	test_non_digit_numeric_positions(void)
{
	const char	*bad_values[] = {"x", "-1", "+5", " 5", "5 ", "1a", "12.5"};
	size_t const	n_bad = sizeof(bad_values) / sizeof(bad_values[0]);
	int			pos;
	size_t			i;

	for (pos = 1; pos <= 7; pos++)
	{
		for (i = 0; i < n_bad; i++)
		{
			char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "fifo"};
			t_config	cfg;
			t_capture	cout;
			t_capture	cerr;
			char		*out_str;
			char		*err_str;

			argv[pos] = (char *)bad_values[i];
			memset(&cfg, 0, sizeof(cfg));
			capture_both_begin(&cout, &cerr);
			TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 9, argv));
			capture_both_end(&cout, &cerr, &out_str, &err_str);
			TEST_ASSERT_TRUE(str_contains(err_str, "INVALID NUMBER"));
			free(out_str);
			free(err_str);
		}
	}
}

/*
* ft_strisdigit() returns 1 for the empty string (vacuously all digits), so
* "" at any numeric position parses fine and stores 0.
*/
static void	test_empty_numeric_string(void)
{
	int		pos;

	for (pos = 1; pos <= 7; pos++)
	{
		char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "fifo"};
		t_config	cfg;
		int			*fields[] = {&cfg.number_of_coders, &cfg.time_to_burnout,
			&cfg.time_to_compile, &cfg.time_to_debug, &cfg.time_to_refactor,
			&cfg.number_of_compiles_required, &cfg.dongle_cooldown};

		argv[pos] = "";
		memset(&cfg, 0, sizeof(cfg));
		TEST_ASSERT_EQUAL_INT(0, parse_arguments(&cfg, 9, argv));
		TEST_ASSERT_EQUAL_INT(0, *fields[pos - 1]);
	}
}

static void	test_huge_number_string(void)
{
	const char	*huge = "99999999999999999999";
	char		*argv[] = {"prog", (char *)huge, "800", "200", "100", "50", "5", "10", "fifo"};
	t_config	cfg;

	memset(&cfg, 0, sizeof(cfg));
	/* ft_strisdigit() accepts any all-digit string, so parsing succeeds and
	 * the field holds whatever atoi() produces for the overflowing value.
	 * The assertion is tautological on purpose: it proves no crash. */
	TEST_ASSERT_EQUAL_INT(0, parse_arguments(&cfg, 9, argv));
	TEST_ASSERT_EQUAL_INT(atoi(huge), cfg.number_of_coders);
}

static void	test_invalid_schedulers(void)
{
	const char	*bad_schedulers[] = {"lifo", "", "FIFO", "EDF", "edfx", "fifo "};
	size_t const	n_bad = sizeof(bad_schedulers) / sizeof(bad_schedulers[0]);
	size_t			i;

	for (i = 0; i < n_bad; i++)
	{
		char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "fifo"};
		t_config	cfg;
		t_capture	cout;
		t_capture	cerr;
		char		*out_str;
		char		*err_str;

		argv[8] = (char *)bad_schedulers[i];
		memset(&cfg, 0, sizeof(cfg));
		capture_both_begin(&cout, &cerr);
		TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 9, argv));
		capture_both_end(&cout, &cerr, &out_str, &err_str);
		TEST_ASSERT_TRUE(str_contains(err_str, "INVALID SCHEDULER"));
		free(out_str);
		free(err_str);
	}
}

/*
* Precedence: with argc 9 the numbers are validated first (argv[1..7]) and the
* scheduler is checked only afterwards. A bad scheduler with valid numbers must
* report INVALID SCHEDULER; a bad number must win over a bad scheduler.
*/
static void	test_precedence_valid_numbers_bad_scheduler(void)
{
	char		*argv[] = {"prog", "3", "800", "200", "100", "50", "5", "10", "lifo"};
	t_config	cfg;
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	memset(&cfg, 0, sizeof(cfg));
	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 9, argv));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	TEST_ASSERT_TRUE(str_contains(err_str, "INVALID SCHEDULER"));
	TEST_ASSERT_FALSE(str_contains(err_str, "INVALID NUMBER"));
	free(out_str);
	free(err_str);
}

static void	test_precedence_bad_number_wins(void)
{
	char		*argv[] = {"prog", "x", "800", "200", "100", "50", "5", "10", "lifo"};
	t_config	cfg;
	t_capture	cout;
	t_capture	cerr;
	char		*out_str;
	char		*err_str;

	memset(&cfg, 0, sizeof(cfg));
	capture_both_begin(&cout, &cerr);
	TEST_ASSERT_EQUAL_INT(-1, parse_arguments(&cfg, 9, argv));
	capture_both_end(&cout, &cerr, &out_str, &err_str);
	TEST_ASSERT_TRUE(str_contains(err_str, "INVALID NUMBER"));
	TEST_ASSERT_FALSE(str_contains(err_str, "INVALID SCHEDULER"));
	free(out_str);
	free(err_str);
}

void	run_parsing_tests(void)
{
	RUN_TEST(test_valid_fifo_args);
	RUN_TEST(test_valid_edf_scheduler);
	RUN_TEST(test_all_zero_arguments);
	RUN_TEST(test_wrong_argc_too_few);
	RUN_TEST(test_wrong_argc_too_many);
	RUN_TEST(test_non_digit_numeric_positions);
	RUN_TEST(test_empty_numeric_string);
	RUN_TEST(test_huge_number_string);
	RUN_TEST(test_invalid_schedulers);
	RUN_TEST(test_precedence_valid_numbers_bad_scheduler);
	RUN_TEST(test_precedence_bad_number_wins);
}

/*
* Unity 2.7 calls setUp()/tearDown() around every test (unity.c) but this
* vendored build provides no weak defaults, so the linker needs real symbols.
* They are empty here and must be defined exactly once per test binary.
*/
void	setUp(void)
{
}

void	tearDown(void)
{
}