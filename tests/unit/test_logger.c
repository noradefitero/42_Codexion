/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_logger.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:47:17 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:19 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"
#include "../../src/models/logger.h"
#include "capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Flushes stdio before starting a capture: capture_end() calls fflush(NULL),
* so buffered Unity PASS/FAIL lines would otherwise leak into the captured
* string.
*/
static int	capture_begin_clean(int fd, t_capture *cap)
{
	fflush(NULL);
	return (capture_begin(fd, cap));
}

/*
* Pool sizing: __pool_size = n_coders * (5 * compiles_required + 2) + 1,
* capped at LOG_POOL_CAP. Each stack logger is reset right after being
* inspected so the queue is freed and the mutex/cond destroyed.
*/
static void	test_pool_formula(void)
{
	t_logger	l;

	memset(&l, 0, sizeof(l));
	TEST_ASSERT_EQUAL_INT(0, logger__init(&l, 1, 1));
	TEST_ASSERT_EQUAL_UINT64(8, l.__pool_size);
	logger__reset(&l);

	memset(&l, 0, sizeof(l));
	TEST_ASSERT_EQUAL_INT(0, logger__init(&l, 2, 1));
	TEST_ASSERT_EQUAL_UINT64(15, l.__pool_size);
	logger__reset(&l);

	memset(&l, 0, sizeof(l));
	TEST_ASSERT_EQUAL_INT(0, logger__init(&l, 1, 2));
	TEST_ASSERT_EQUAL_UINT64(13, l.__pool_size);
	logger__reset(&l);

	memset(&l, 0, sizeof(l));
	TEST_ASSERT_EQUAL_INT(0, logger__init(&l, 2, 2));
	TEST_ASSERT_EQUAL_UINT64(25, l.__pool_size);
	logger__reset(&l);

	/* huge compiles_required: the size is clamped to LOG_POOL_CAP */
	memset(&l, 0, sizeof(l));
	TEST_ASSERT_EQUAL_INT(0, logger__init(&l, 1, 100000));
	TEST_ASSERT_EQUAL_UINT64(LOG_POOL_CAP, l.__pool_size);
	logger__reset(&l);
}

static void	test_create_and_destroy(void)
{
	t_logger	*l;

	l = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(l);
	logger__destroy(l);

	/* destroying NULL is a safe no-op */
	logger__destroy(NULL);
}

static void	test_add_pop_roundtrip(void)
{
	t_logger	*l;
	t_log		buf;

	l = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(l);

	logger__add_to_queue(l, 7, COMPILING);
	logger__add_to_queue(l, 9, DEBUGGING);

	TEST_ASSERT_EQUAL_INT(1, logger__pop_queue(l, &buf));
	TEST_ASSERT_EQUAL_INT(7, buf.coder_id);
	TEST_ASSERT_EQUAL_INT(COMPILING, buf.state);
	TEST_ASSERT_TRUE(buf.timestamp >= 0);

	TEST_ASSERT_EQUAL_INT(1, logger__pop_queue(l, &buf));
	TEST_ASSERT_EQUAL_INT(9, buf.coder_id);
	TEST_ASSERT_EQUAL_INT(DEBUGGING, buf.state);
	TEST_ASSERT_TRUE(buf.timestamp >= 0);

	/* FIFO roundtrip is complete: the queue is empty again */
	TEST_ASSERT_EQUAL_UINT64(0, l->__size);
	logger__destroy(l);
}

/*
* pop_queue() on an empty queue with no stop requested BLOCKS forever, so it
* is never called in that state. After request_stop() the pop returns 0 at
* once, even with an empty queue.
*/
static void	test_pop_empty_after_stop(void)
{
	t_logger	*l;
	t_log		buf;

	l = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(l);
	logger__request_stop(l);
	TEST_ASSERT_EQUAL_INT(0, logger__pop_queue(l, &buf));
	logger__destroy(l);
}

/*
* logger__wake() on an initialized logger must not crash or change queue
* state, and a second request_stop() must be harmless.
*/
static void	test_wake_and_double_stop(void)
{
	t_logger	*l;
	t_log		buf;

	l = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(l);
	logger__wake(l);
	TEST_ASSERT_EQUAL_UINT64(0, l->__size);
	logger__request_stop(l);
	logger__request_stop(l);
	TEST_ASSERT_EQUAL_INT(0, logger__pop_queue(l, &buf));
	logger__destroy(l);
}

/*
* Growth / overflow stress. Pool is 8. Push ids 0..7 (fills the pool), pop 3
* (ids 0..2, so __head advances mid-ring), then push 25 more ids 8..32.
* The pushes hit a full pool with a wrapped ring, forcing several
* logger__increase_pool() re-linearizations. FIFO order is preserved:
* the 30 remaining logs come back as ids 3..32, in order, with the state
* cycling TAKEN_DONGLE/COMPILING/DEBUGGING/REFACTORING (state = id % 4).
*/
static void	test_queue_growth_stress(void)
{
	t_logger	*l;
	t_log		buf;
	int			i;

	l = logger__create(1, 1); /* pool 8 */
	TEST_ASSERT_NOT_NULL(l);

	for (i = 0; i < 8; i++)
		logger__add_to_queue(l, i, (t_log_mess)(i % 4));

	for (i = 0; i < 3; i++)
	{
		TEST_ASSERT_EQUAL_INT(1, logger__pop_queue(l, &buf));
		TEST_ASSERT_EQUAL_INT(i, buf.coder_id);
	}

	for (i = 8; i < 33; i++)
		logger__add_to_queue(l, i, (t_log_mess)(i % 4));

	for (i = 3; i < 33; i++)
	{
		TEST_ASSERT_EQUAL_INT(1, logger__pop_queue(l, &buf));
		TEST_ASSERT_EQUAL_INT(i, buf.coder_id);
		TEST_ASSERT_EQUAL_INT(i % 4, buf.state);
	}
	TEST_ASSERT_EQUAL_UINT64(0, l->__size);
	logger__destroy(l);
}

/*
* Full thread lifecycle: spawn the draining thread, feed it 5 logs, request
* stop, join. After the join every log has been drained (popped and printed),
* __size is 0 and a further pop returns 0. The capture swallows whatever the
* thread prints via log_state(). Note: by the time this suite runs, the print
* suite has latched the process-global `burned` flag, so the thread's
* log_state() output is suppressed anyway — do NOT assert on it here.
*/
static void	test_thread_lifecycle(void)
{
	t_logger	*l;
	t_log		buf;
	t_capture	cap;
	char		*out;
	int			i;

	l = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(l);

	capture_begin_clean(1, &cap);
	TEST_ASSERT_EQUAL_INT(0, logger__init_thread(l));
	for (i = 0; i < 5; i++)
		logger__add_to_queue(l, i, (t_log_mess)(i % 4));
	logger__request_stop(l);
	TEST_ASSERT_EQUAL_INT(0, logger__join_thread(l));
	out = capture_end(&cap);
	TEST_ASSERT_NOT_NULL(out);
	free(out);

	TEST_ASSERT_EQUAL_UINT64(0, l->__size);
	TEST_ASSERT_EQUAL_INT(0, logger__pop_queue(l, &buf));
	logger__destroy(l);
}

void	run_logger_tests(void)
{
	RUN_TEST(test_pool_formula);
	RUN_TEST(test_create_and_destroy);
	RUN_TEST(test_add_pop_roundtrip);
	RUN_TEST(test_pop_empty_after_stop);
	RUN_TEST(test_wake_and_double_stop);
	RUN_TEST(test_queue_growth_stress);
	RUN_TEST(test_thread_lifecycle);
}