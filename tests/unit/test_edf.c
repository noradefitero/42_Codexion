/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_edf.c                                         :+:      :+:    :+:   */
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
#include "../../src/models/schedulers/edf.h"
#include "../../src/models/schedulers/scheduler.h"
#include "../../src/models/schedulers/scheduler_getters.h"
#include "../../src/models/coder.h"

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

static void test_edf_init_stack(void)
{
	t_edf	e;

	edf__init(&e);
	TEST_ASSERT_EQUAL(0, e.__head);
	TEST_ASSERT_EQUAL(0, e.__tail);
	TEST_ASSERT_EQUAL(0, e.__size);
	TEST_ASSERT_NULL(e.__queue[0]);
	TEST_ASSERT_NULL(e.__queue[1]);
	TEST_ASSERT_NULL(edf__first(&e));
	TEST_ASSERT_EQUAL_PTR(&e, e.super.__self);
	TEST_ASSERT_NOT_NULL(e.super.__put);
	TEST_ASSERT_NOT_NULL(e.super.__pop);
	TEST_ASSERT_NOT_NULL(e.super.__delete);
	TEST_ASSERT_NOT_NULL(e.super.__first);
	TEST_ASSERT_NOT_NULL(e.super.__reset);
	TEST_ASSERT_NOT_NULL(e.super.__destroy);
	TEST_ASSERT_NULL(scheduler__first((t_scheduler *)&e));
	TEST_ASSERT_EQUAL(e.__head, edf__at(&e, 0));
	TEST_ASSERT_EQUAL((e.__head + 1) % MAX_CODERS, edf__at(&e, 1));
	TEST_ASSERT_EQUAL((e.__head + 2) % MAX_CODERS, edf__at(&e, 2));
}

static void test_edf_create_destroy(void)
{
	t_edf	*e;
	t_edf	*null_e;

	e = edf__create();
	TEST_ASSERT_NOT_NULL(e);
	edf__destroy(e);

	/* NULL destroy must be a safe no-op. */
	null_e = NULL;
	edf__destroy(null_e);

	/* Destroy through the scheduler vtable. */
	e = edf__create();
	TEST_ASSERT_NOT_NULL(e);
	scheduler__destroy((t_scheduler *)e);
}

static void test_edf_ordering(void)
{
	t_edf	e;
	t_coder	*d50;
	t_coder	*d10;

	d50 = mk_coder(1, 50);
	d10 = mk_coder(2, 10);
	edf__init(&e);

	TEST_ASSERT_EQUAL(0, edf__put(&e, d50));
	TEST_ASSERT_EQUAL_PTR(d50, edf__first(&e));

	/* Min-heap by deadline: the smaller deadline becomes first. */
	TEST_ASSERT_EQUAL(0, edf__put(&e, d10));
	TEST_ASSERT_EQUAL_PTR(d10, edf__first(&e));

	/* Pop only removes the head (pass the head coder). */
	edf__pop(&e, d10);
	TEST_ASSERT_EQUAL_PTR(d50, edf__first(&e));
	TEST_ASSERT_EQUAL(1, e.__size);

	coder__destroy(d10);
	coder__destroy(d50);
}

static void test_edf_pop(void)
{
	t_edf	e;
	t_coder	*d50;
	t_coder	*d10;

	d50 = mk_coder(1, 50);
	d10 = mk_coder(2, 10);
	edf__init(&e);
	edf__put(&e, d50);
	edf__put(&e, d10);

	/* Pop of a non-head element is a no-op. */
	edf__pop(&e, d50);
	TEST_ASSERT_EQUAL(2, e.__size);
	TEST_ASSERT_EQUAL_PTR(d10, edf__first(&e));

	edf__pop(&e, d10);
	TEST_ASSERT_EQUAL_PTR(d50, edf__first(&e));
	TEST_ASSERT_EQUAL(1, e.__size);

	edf__pop(&e, d50);
	TEST_ASSERT_EQUAL(0, e.__size);
	TEST_ASSERT_NULL(edf__first(&e));

	/* Pop on an empty queue is a no-op. */
	edf__pop(&e, d10);
	TEST_ASSERT_EQUAL(0, e.__size);
	TEST_ASSERT_NULL(edf__first(&e));

	coder__destroy(d10);
	coder__destroy(d50);
}

static void test_edf_ties(void)
{
	t_edf	e;
	t_coder	*a;
	t_coder	*b;

	a = mk_coder(1, 10);
	b = mk_coder(2, 10);
	edf__init(&e);

	/* Tie on deadline: strict < keeps insertion order, A stays first. */
	edf__put(&e, a);
	edf__put(&e, b);
	TEST_ASSERT_EQUAL_PTR(a, edf__first(&e));

	edf__pop(&e, a);
	TEST_ASSERT_EQUAL_PTR(b, edf__first(&e));

	coder__destroy(b);
	coder__destroy(a);
}

static void test_edf_wrap(void)
{
	t_edf	e;
	t_coder	*a50;
	t_coder	*b10;
	t_coder	*c30;

	a50 = mk_coder(1, 50);
	b10 = mk_coder(2, 10);
	c30 = mk_coder(3, 30);
	edf__init(&e);

	edf__put(&e, a50);
	edf__put(&e, b10);
	TEST_ASSERT_EQUAL_PTR(b10, edf__first(&e));

	edf__pop(&e, b10);
	TEST_ASSERT_EQUAL_PTR(a50, edf__first(&e));

	edf__put(&e, c30);
	TEST_ASSERT_EQUAL_PTR(c30, edf__first(&e));

	edf__pop(&e, c30);
	TEST_ASSERT_EQUAL_PTR(a50, edf__first(&e));

	edf__pop(&e, a50);
	TEST_ASSERT_EQUAL(0, e.__size);
	TEST_ASSERT_NULL(edf__first(&e));

	coder__destroy(c30);
	coder__destroy(b10);
	coder__destroy(a50);
}

static void test_edf_delete(void)
{
	t_edf	e;
	t_coder	*a10;
	t_coder	*b50;
	t_coder	*absent;

	a10 = mk_coder(1, 10);
	b50 = mk_coder(2, 50);
	absent = mk_coder(3, 40);
	edf__init(&e);

	edf__put(&e, a10);
	edf__put(&e, b50);
	edf__delete(&e, b50);
	TEST_ASSERT_EQUAL_PTR(a10, edf__first(&e));
	TEST_ASSERT_EQUAL(1, e.__size);

	/* Deleting an absent coder is a no-op. */
	edf__delete(&e, absent);
	TEST_ASSERT_EQUAL(1, e.__size);
	TEST_ASSERT_EQUAL_PTR(a10, edf__first(&e));

	edf__delete(&e, a10);
	TEST_ASSERT_EQUAL(0, e.__size);
	TEST_ASSERT_NULL(edf__first(&e));

	coder__destroy(absent);
	coder__destroy(b50);
	coder__destroy(a10);
}

static void test_edf_delete_head(void)
{
	t_edf	e;
	t_coder	*a50;
	t_coder	*b10;

	a50 = mk_coder(1, 50);
	b10 = mk_coder(2, 10);
	edf__init(&e);

	edf__put(&e, a50);
	edf__put(&e, b10);
	TEST_ASSERT_EQUAL_PTR(b10, edf__first(&e));

	/* Delete A(50): heap keeps B(10) as first. */
	edf__delete(&e, a50);
	TEST_ASSERT_EQUAL_PTR(b10, edf__first(&e));
	TEST_ASSERT_EQUAL(1, e.__size);

	coder__destroy(b10);
	coder__destroy(a50);
}

static void test_edf_sift_up_direct(void)
{
	t_edf	e;
	t_coder	*big;
	t_coder	*small;

	big = mk_coder(1, 50);
	small = mk_coder(2, 10);
	edf__init(&e);

	/* Hand-build a broken heap: big at root, small at index 1. */
	e.__queue[e.__head] = big;
	e.__queue[edf__at(&e, 1)] = small;
	e.__size = 2;
	e.__tail = edf__at(&e, 2);

	edf__sift_up(&e, 1);
	TEST_ASSERT_EQUAL_PTR(small, edf__first(&e));

	coder__destroy(small);
	coder__destroy(big);
}

static void test_edf_sift_down_direct(void)
{
	t_edf	e;
	t_coder	*big;
	t_coder	*small;

	big = mk_coder(1, 50);
	small = mk_coder(2, 10);
	edf__init(&e);

	/* Hand-build a broken heap: big at root, small at index 1. */
	e.__queue[e.__head] = big;
	e.__queue[edf__at(&e, 1)] = small;
	e.__size = 2;

	edf__sift_down(&e, 0);
	TEST_ASSERT_EQUAL_PTR(small, edf__first(&e));

	coder__destroy(small);
	coder__destroy(big);
}

static void test_edf_full(void)
{
	t_edf	e;
	t_coder	*a;
	t_coder	*b;
	t_coder	*c;

	a = mk_coder(1, 30);
	b = mk_coder(2, 10);
	c = mk_coder(3, 20);
	edf__init(&e);

	TEST_ASSERT_EQUAL(0, edf__put(&e, a));
	TEST_ASSERT_EQUAL(0, edf__put(&e, b));
	TEST_ASSERT_EQUAL_PTR(b, edf__first(&e));

	/* Third put: queue full (MAX_CODERS == 2). */
	TEST_ASSERT_EQUAL(1, edf__put(&e, c));
	TEST_ASSERT_EQUAL(2, e.__size);
	TEST_ASSERT_EQUAL_PTR(b, edf__first(&e));

	coder__destroy(c);
	coder__destroy(b);
	coder__destroy(a);
}

static void test_edf_vtable_dispatch(void)
{
	t_edf		e;
	t_scheduler	*s;
	t_coder		*d50;
	t_coder		*d10;

	s = (t_scheduler *)&e;
	d50 = mk_coder(1, 50);
	d10 = mk_coder(2, 10);
	edf__init(&e);

	TEST_ASSERT_EQUAL(0, scheduler__put(s, d50));
	TEST_ASSERT_EQUAL(0, scheduler__put(s, d10));
	TEST_ASSERT_EQUAL_PTR(d10, scheduler__first(s));

	scheduler__pop(s, d10);
	TEST_ASSERT_EQUAL_PTR(d50, scheduler__first(s));

	scheduler__delete(s, d50);
	TEST_ASSERT_NULL(scheduler__first(s));

	scheduler__reset(s);
	TEST_ASSERT_NULL(scheduler__first(s));
	TEST_ASSERT_EQUAL(0, scheduler__put(s, d50));
	TEST_ASSERT_EQUAL_PTR(d50, scheduler__first(s));

	coder__destroy(d10);
	coder__destroy(d50);
}

void	run_edf_tests(void)
{
	RUN_TEST(test_edf_init_stack);
	RUN_TEST(test_edf_create_destroy);
	RUN_TEST(test_edf_ordering);
	RUN_TEST(test_edf_pop);
	RUN_TEST(test_edf_ties);
	RUN_TEST(test_edf_wrap);
	RUN_TEST(test_edf_delete);
	RUN_TEST(test_edf_delete_head);
	RUN_TEST(test_edf_sift_up_direct);
	RUN_TEST(test_edf_sift_down_direct);
	RUN_TEST(test_edf_full);
	RUN_TEST(test_edf_vtable_dispatch);
}