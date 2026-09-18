/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_fifo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 12:00:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#define MAX_CODERS 2
#include "../../Unity/src/unity.h"
#include "../../src/models/schedulers/fifo.h"
#include "../../src/models/schedulers/scheduler.h"
#include "../../src/models/schedulers/scheduler_getters.h"
#include "../../src/models/coder.h"

/* Unity lifecycle hooks (defined once for the test_sched binary). */
void	setUp(void)
{
}

void	tearDown(void)
{
}

static t_coder *mk_coder(int id, t_ms deadline)
{
	t_config	cfg;
	t_coder		*c;

	cfg.scheduler = FIFO;
	c = coder__create(id, &cfg, NULL, NULL);
	c->__last_compile = 0;
	c->__time_to_burnout = (int)deadline;
	return (c);
}

static void test_fifo_init_stack(void)
{
	t_fifo	f;

	fifo__init(&f);
	TEST_ASSERT_EQUAL(0, f.__head);
	TEST_ASSERT_EQUAL(0, f.__tail);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(f.__queue[0]);
	TEST_ASSERT_NULL(f.__queue[1]);
	TEST_ASSERT_NULL(fifo__first(&f));
	TEST_ASSERT_EQUAL_PTR(&f, f.super.__self);
	TEST_ASSERT_NOT_NULL(f.super.__put);
	TEST_ASSERT_NOT_NULL(f.super.__pop);
	TEST_ASSERT_NOT_NULL(f.super.__delete);
	TEST_ASSERT_NOT_NULL(f.super.__first);
	TEST_ASSERT_NOT_NULL(f.super.__reset);
	TEST_ASSERT_NOT_NULL(f.super.__destroy);
	TEST_ASSERT_NULL(scheduler__first((t_scheduler *)&f));
}

static void test_fifo_create_destroy(void)
{
	t_fifo	*f;
	t_fifo	*null_f;

	f = fifo__create();
	TEST_ASSERT_NOT_NULL(f);
	fifo__destroy(f);

	/* NULL destroy must be a safe no-op. */
	null_f = NULL;
	fifo__destroy(null_f);

	/* Destroy through the scheduler vtable. */
	f = fifo__create();
	TEST_ASSERT_NOT_NULL(f);
	scheduler__destroy((t_scheduler *)f);
}

static void test_fifo_put(void)
{
	t_fifo	f;
	t_coder	*c1;
	t_coder	*c2;
	t_coder	*c3;

	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);
	fifo__init(&f);

	TEST_ASSERT_EQUAL(0, fifo__put(&f, c1));
	TEST_ASSERT_EQUAL_PTR(c1, fifo__first(&f));
	TEST_ASSERT_EQUAL(1, f.__size);

	TEST_ASSERT_EQUAL(0, fifo__put(&f, c2));
	TEST_ASSERT_EQUAL(2, f.__size);

	/* Third put: queue full (MAX_CODERS == 2). */
	TEST_ASSERT_EQUAL(1, fifo__put(&f, c3));
	TEST_ASSERT_EQUAL(2, f.__size);
	TEST_ASSERT_EQUAL_PTR(c1, fifo__first(&f));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
}

static void test_fifo_pop(void)
{
	t_fifo	f;
	t_coder	*c1;
	t_coder	*c2;

	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	fifo__init(&f);
	fifo__put(&f, c1);
	fifo__put(&f, c2);

	/* Pop of a non-head element is a no-op. */
	fifo__pop(&f, c2);
	TEST_ASSERT_EQUAL(2, f.__size);
	TEST_ASSERT_EQUAL_PTR(c1, fifo__first(&f));

	fifo__pop(&f, c1);
	TEST_ASSERT_EQUAL(1, f.__size);
	TEST_ASSERT_EQUAL_PTR(c2, fifo__first(&f));

	fifo__pop(&f, c2);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(fifo__first(&f));

	/* Pop on an empty queue is a no-op. */
	fifo__pop(&f, c1);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(fifo__first(&f));

	coder__destroy(c2);
	coder__destroy(c1);
}

static void test_fifo_wrap_around(void)
{
	t_fifo	f;
	t_coder	*c1;
	t_coder	*c2;
	t_coder	*c3;
	t_coder	*c4;

	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);
	c4 = mk_coder(4, 40);
	fifo__init(&f);

	fifo__put(&f, c1);
	fifo__put(&f, c2);
	fifo__pop(&f, c1);
	TEST_ASSERT_EQUAL_PTR(c2, fifo__first(&f));

	fifo__put(&f, c3);
	TEST_ASSERT_EQUAL(2, f.__size);
	TEST_ASSERT_EQUAL_PTR(c2, fifo__first(&f));

	fifo__pop(&f, c2);
	TEST_ASSERT_EQUAL_PTR(c3, fifo__first(&f));

	fifo__pop(&f, c3);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(fifo__first(&f));

	/* Full drain: the ring is usable again. */
	TEST_ASSERT_EQUAL(0, fifo__put(&f, c4));
	TEST_ASSERT_EQUAL(1, f.__size);
	TEST_ASSERT_EQUAL_PTR(c4, fifo__first(&f));

	coder__destroy(c4);
	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
}

static void test_fifo_delete(void)
{
	t_fifo	f;
	t_coder	*c1;
	t_coder	*c2;
	t_coder	*c3;

	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);
	fifo__init(&f);

	fifo__put(&f, c1);
	fifo__put(&f, c2);
	fifo__delete(&f, c1);
	TEST_ASSERT_EQUAL_PTR(c2, fifo__first(&f));
	TEST_ASSERT_EQUAL(1, f.__size);

	/* Deleting an absent coder is a no-op. */
	fifo__delete(&f, c3);
	TEST_ASSERT_EQUAL(1, f.__size);
	TEST_ASSERT_EQUAL_PTR(c2, fifo__first(&f));

	/* Delete the last element. */
	fifo__delete(&f, c2);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(fifo__first(&f));

	/* Delete everything, then re-put works. */
	fifo__put(&f, c1);
	fifo__put(&f, c2);
	fifo__delete(&f, c1);
	fifo__delete(&f, c2);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_EQUAL(0, fifo__put(&f, c3));
	TEST_ASSERT_EQUAL_PTR(c3, fifo__first(&f));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
}

static void test_fifo_reset(void)
{
	t_fifo	f;
	t_coder	*c1;
	t_coder	*c2;
	t_coder	*c3;

	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);
	fifo__init(&f);

	fifo__put(&f, c1);
	fifo__put(&f, c2);
	fifo__reset(&f);
	TEST_ASSERT_EQUAL(0, f.__size);
	TEST_ASSERT_NULL(fifo__first(&f));

	TEST_ASSERT_EQUAL(0, fifo__put(&f, c3));
	TEST_ASSERT_EQUAL_PTR(c3, fifo__first(&f));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
}

static void test_fifo_vtable_dispatch(void)
{
	t_fifo		f;
	t_scheduler	*s;
	t_coder		*c1;
	t_coder		*c2;
	t_coder		*c3;

	s = (t_scheduler *)&f;
	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);
	fifo__init(&f);

	TEST_ASSERT_EQUAL(0, scheduler__put(s, c1));
	TEST_ASSERT_EQUAL_PTR(c1, scheduler__first(s));
	TEST_ASSERT_EQUAL(0, scheduler__put(s, c2));

	scheduler__pop(s, c2);
	TEST_ASSERT_EQUAL_PTR(c1, scheduler__first(s));
	scheduler__pop(s, c1);
	TEST_ASSERT_EQUAL_PTR(c2, scheduler__first(s));

	scheduler__delete(s, c2);
	TEST_ASSERT_NULL(scheduler__first(s));

	scheduler__reset(s);
	TEST_ASSERT_NULL(scheduler__first(s));
	TEST_ASSERT_EQUAL(0, scheduler__put(s, c3));
	TEST_ASSERT_EQUAL_PTR(c3, scheduler__first(s));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
}

void	run_fifo_tests(void)
{
	RUN_TEST(test_fifo_init_stack);
	RUN_TEST(test_fifo_create_destroy);
	RUN_TEST(test_fifo_put);
	RUN_TEST(test_fifo_pop);
	RUN_TEST(test_fifo_wrap_around);
	RUN_TEST(test_fifo_delete);
	RUN_TEST(test_fifo_reset);
	RUN_TEST(test_fifo_vtable_dispatch);
}