/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_time.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:47:47 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:53 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/utils/time.h"

static void	assert_timeval_eq(const struct timeval *expected,
	const struct timeval *actual)
{
	TEST_ASSERT_EQUAL_INT64((long long)expected->tv_sec,
		(long long)actual->tv_sec);
	TEST_ASSERT_EQUAL_INT64((long long)expected->tv_usec,
		(long long)actual->tv_usec);
}

static void	assert_timespec_eq(const struct timespec *expected,
	const struct timespec *actual)
{
	TEST_ASSERT_EQUAL_INT64((long long)expected->tv_sec,
		(long long)actual->tv_sec);
	TEST_ASSERT_EQUAL_INT64((long long)expected->tv_nsec,
		(long long)actual->tv_nsec);
}

static void	test_timeval_to_timespec(void)
{
	struct timeval	tv;
	struct timespec	ts;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	ts = timeval_to_timespec(&tv);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_nsec);

	tv.tv_sec = 5;
	tv.tv_usec = 123456;
	ts = timeval_to_timespec(&tv);
	TEST_ASSERT_EQUAL_INT64(5, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(123456000, (long long)ts.tv_nsec);

	tv.tv_sec = 2;
	tv.tv_usec = 999999;
	ts = timeval_to_timespec(&tv);
	TEST_ASSERT_EQUAL_INT64(2, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(999999000, (long long)ts.tv_nsec);
}

static void	test_timespec_to_timeval(void)
{
	struct timespec	ts;
	struct timeval	tv;

	ts.tv_sec = 5;
	ts.tv_nsec = 123456000;
	tv = timespec_to_timeval(&ts);
	TEST_ASSERT_EQUAL_INT64(5, (long long)tv.tv_sec);
	TEST_ASSERT_EQUAL_INT64(123456, (long long)tv.tv_usec);

	ts.tv_sec = 0;
	ts.tv_nsec = 999;
	tv = timespec_to_timeval(&ts);
	TEST_ASSERT_EQUAL_INT64(0, (long long)tv.tv_sec);
	TEST_ASSERT_EQUAL_INT64(0, (long long)tv.tv_usec);

	ts.tv_sec = 0;
	ts.tv_nsec = 1000;
	tv = timespec_to_timeval(&ts);
	TEST_ASSERT_EQUAL_INT64(0, (long long)tv.tv_sec);
	TEST_ASSERT_EQUAL_INT64(1, (long long)tv.tv_usec);

	ts.tv_sec = 2;
	ts.tv_nsec = 999999999;
	tv = timespec_to_timeval(&ts);
	TEST_ASSERT_EQUAL_INT64(2, (long long)tv.tv_sec);
	TEST_ASSERT_EQUAL_INT64(999999, (long long)tv.tv_usec);
}

static void	test_timeval_to_ms(void)
{
	struct timeval	tv;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	TEST_ASSERT_EQUAL_INT64(0, (long long)timeval_to_ms(&tv));

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	TEST_ASSERT_EQUAL_INT64(1000, (long long)timeval_to_ms(&tv));

	tv.tv_sec = 0;
	tv.tv_usec = 999;
	TEST_ASSERT_EQUAL_INT64(0, (long long)timeval_to_ms(&tv));

	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	TEST_ASSERT_EQUAL_INT64(1, (long long)timeval_to_ms(&tv));

	tv.tv_sec = 0;
	tv.tv_usec = 456789;
	TEST_ASSERT_EQUAL_INT64(456, (long long)timeval_to_ms(&tv));

	tv.tv_sec = 12;
	tv.tv_usec = 345678;
	TEST_ASSERT_EQUAL_INT64(12345, (long long)timeval_to_ms(&tv));
}

static void	test_ms_to_timespec(void)
{
	struct timespec	ts;

	ts = ms_to_timespec(0);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_nsec);

	ts = ms_to_timespec(1);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(1000000, (long long)ts.tv_nsec);

	ts = ms_to_timespec(999);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(999000000, (long long)ts.tv_nsec);

	ts = ms_to_timespec(1000);
	TEST_ASSERT_EQUAL_INT64(1, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(0, (long long)ts.tv_nsec);

	ts = ms_to_timespec(1001);
	TEST_ASSERT_EQUAL_INT64(1, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(1000000, (long long)ts.tv_nsec);

	ts = ms_to_timespec(1234567);
	TEST_ASSERT_EQUAL_INT64(1234, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64(567000000, (long long)ts.tv_nsec);
}

static void	test_roundtrip_tv_ts_tv(void)
{
	struct timeval	tv;
	struct timeval	back;
	struct timespec	ts;

	/* usec is preserved exactly through tv -> ts -> tv */
	tv.tv_sec = 5;
	tv.tv_usec = 123456;
	ts = timeval_to_timespec(&tv);
	back = timespec_to_timeval(&ts);
	assert_timeval_eq(&tv, &back);
}

static void	test_roundtrip_tv_ms_ts(void)
{
	struct timeval	tv;
	struct timespec	ts;
	t_ms			ms;

	tv.tv_sec = 12;
	tv.tv_usec = 345678;
	ms = timeval_to_ms(&tv);
	ts = ms_to_timespec(ms);
	TEST_ASSERT_EQUAL_INT64((long long)tv.tv_sec, (long long)ts.tv_sec);
	TEST_ASSERT_EQUAL_INT64((long long)((tv.tv_usec / 1000) * 1000000),
		(long long)ts.tv_nsec);
}

static void	test_roundtrip_ts_tv_ts(void)
{
	struct timespec	ts;
	struct timespec	expected;
	struct timeval	tv;
	struct timespec	back;

	/* nsec is truncated to a multiple of 1000 through ts -> tv -> ts */
	ts.tv_sec = 2;
	ts.tv_nsec = 999999999;
	expected.tv_sec = 2;
	expected.tv_nsec = 999999000;
	tv = timespec_to_timeval(&ts);
	back = timeval_to_timespec(&tv);
	assert_timespec_eq(&expected, &back);
}

static void	test_get_sim_time_reset(void)
{
	t_ms	v;

	/* resetting the base yields ~0 (allow +/-5ms slack) */
	v = get_sim_time(true);
	TEST_ASSERT_TRUE(v >= 0);
	TEST_ASSERT_TRUE(v <= 5);
}

static void	test_get_sim_time_after_sleep(void)
{
	t_ms	v;

	get_sim_time(true);
	sleep_ms(20);
	v = get_sim_time(false);
	TEST_ASSERT_TRUE(v > 5);
	TEST_ASSERT_TRUE(v < 500);
}

static void	test_get_sim_time_monotonic(void)
{
	t_ms	t1;
	t_ms	t2;
	t_ms	t3;

	get_sim_time(true);
	t1 = get_sim_time(false);
	sleep_ms(1);
	t2 = get_sim_time(false);
	sleep_ms(1);
	t3 = get_sim_time(false);
	TEST_ASSERT_TRUE(t1 <= t2);
	TEST_ASSERT_TRUE(t2 <= t3);
}

static void	test_get_sim_time_reset_again(void)
{
	t_ms	v;

	v = get_sim_time(true);
	TEST_ASSERT_TRUE(v >= 0);
	TEST_ASSERT_TRUE(v <= 5);
}

static void	test_sleep_ms_zero(void)
{
	t_ms	start;
	t_ms	elapsed;

	start = get_time();
	sleep_ms(0);
	elapsed = get_time() - start;
	TEST_ASSERT_TRUE(elapsed < 50);
}

static void	test_sleep_ms_ten(void)
{
	t_ms	start;
	t_ms	elapsed;

	start = get_time();
	sleep_ms(10);
	elapsed = get_time() - start;
	TEST_ASSERT_TRUE(elapsed >= 8);
	TEST_ASSERT_TRUE(elapsed < 500);
}

static void	test_sleep_ms_long(void)
{
	t_ms	start;
	t_ms	elapsed;

	start = get_time();
	sleep_ms(1500);
	elapsed = get_time() - start;
	TEST_ASSERT_TRUE(elapsed >= 1400);
	TEST_ASSERT_TRUE(elapsed < 5000);
}

void	run_time_tests(void)
{
	RUN_TEST(test_timeval_to_timespec);
	RUN_TEST(test_timespec_to_timeval);
	RUN_TEST(test_timeval_to_ms);
	RUN_TEST(test_ms_to_timespec);
	RUN_TEST(test_roundtrip_tv_ts_tv);
	RUN_TEST(test_roundtrip_tv_ms_ts);
	RUN_TEST(test_roundtrip_ts_tv_ts);
	RUN_TEST(test_get_sim_time_reset);
	RUN_TEST(test_get_sim_time_after_sleep);
	RUN_TEST(test_get_sim_time_monotonic);
	RUN_TEST(test_get_sim_time_reset_again);
	RUN_TEST(test_sleep_ms_zero);
	RUN_TEST(test_sleep_ms_ten);
	RUN_TEST(test_sleep_ms_long);
}