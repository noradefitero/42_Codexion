/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_usb.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:55 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#define MAX_CODERS 2
#include "../../Unity/src/unity.h"
#include "../../src/models/usb.h"
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

static t_config mk_config(t_scheduler_type sched, int cooldown)
{
	t_config	cfg;

	cfg.scheduler = sched;
	cfg.dongle_cooldown = cooldown;
	return (cfg);
}

static void test_usb_create_fifo(void)
{
	t_config	cfg;
	t_usb		*u;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_TRUE(u->__active);
	TEST_ASSERT_NULL(usb__first(u));
	TEST_ASSERT_NULL(usb__holder(u));
	TEST_ASSERT_NOT_NULL(usb__mutex(u));
	TEST_ASSERT_NOT_NULL(usb__cond(u));
	usb__destroy(u);
}

static void test_usb_create_edf(void)
{
	t_config	cfg;
	t_usb		*u;
	t_coder		*c1;
	t_coder		*c2;

	cfg = mk_config(EDF, 0);
	u = usb__create(&cfg);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_TRUE(u->__active);

	/* With an EDF scheduler the first() honors deadlines. */
	c1 = mk_coder(1, 50);
	c2 = mk_coder(2, 10);
	TEST_ASSERT_EQUAL(0, usb__acquire(u, c1));
	TEST_ASSERT_EQUAL(0, usb__acquire(u, c2));
	TEST_ASSERT_EQUAL_PTR(c2, usb__first(u));

	coder__destroy(c2);
	coder__destroy(c1);
	usb__destroy(u);
}

static void test_usb_create_unknown(void)
{
	t_config	cfg;
	t_usb		*u;

	cfg = mk_config(UNKNOWN, 0);
	u = usb__create(&cfg);
	TEST_ASSERT_NULL(u);
}

static void test_usb_stack_init_reset(void)
{
	t_config	cfg;
	t_usb		u;

	cfg = mk_config(FIFO, 0);
	TEST_ASSERT_EQUAL(0, usb__init(&u, &cfg));
	TEST_ASSERT_TRUE(u.__active);
	TEST_ASSERT_NULL(usb__first(&u));
	TEST_ASSERT_NULL(usb__holder(&u));

	/* usb__reset (NOT usb__destroy) on a stack object. */
	usb__reset(&u);
	TEST_ASSERT_FALSE(u.__active);
	TEST_ASSERT_NULL(u.__scheduler);
}

static void test_usb_cooldown_deadline(void)
{
	t_config	cfg;
	t_usb		u;

	cfg = mk_config(FIFO, 50);
	usb__init(&u, &cfg);

	/* cooldown 50, __last_used -1 → 49. */
	u.__last_used = -1;
	TEST_ASSERT_EQUAL(49, (int)usb__cooldown_deadline(&u));

	/* cooldown 50, __last_used 100 → 150. */
	u.__last_used = 100;
	TEST_ASSERT_EQUAL(150, (int)usb__cooldown_deadline(&u));

	/* cooldown 0 → 0 (and negative cooldown → 0). */
	u.__dongle_cooldown = 0;
	TEST_ASSERT_EQUAL(0, (int)usb__cooldown_deadline(&u));
	u.__dongle_cooldown = -5;
	TEST_ASSERT_EQUAL(0, (int)usb__cooldown_deadline(&u));

	usb__reset(&u);
}

static void test_usb_acquire(void)
{
	t_config	cfg;
	t_usb		*u;
	t_coder		*c1;
	t_coder		*c2;
	t_coder		*c3;
	t_fifo		*f;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);

	TEST_ASSERT_EQUAL(0, usb__acquire(u, c1));
	TEST_ASSERT_EQUAL_PTR(c1, usb__first(u));
	/* Holder is only ever set by the coder thread fn. */
	TEST_ASSERT_NULL(usb__holder(u));

	/* Two coders queued → scheduler full. */
	TEST_ASSERT_EQUAL(0, usb__acquire(u, c2));
	f = (t_fifo *)u->__scheduler->__self;
	TEST_ASSERT_EQUAL(2, f->__size);

	/* Third acquire fails. */
	TEST_ASSERT_EQUAL(1, usb__acquire(u, c3));
	TEST_ASSERT_EQUAL_PTR(c1, usb__first(u));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
	usb__destroy(u);
}

static void test_usb_release_safe(void)
{
	t_config	cfg;
	t_usb		*u;
	t_coder		*c1;
	t_coder		*c2;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);

	/* cooldown 0: holder cleared, __last_used untouched (-1). */
	u->__holder = c1;
	usb__release_safe(u, c1);
	TEST_ASSERT_NULL(usb__holder(u));
	TEST_ASSERT_EQUAL(-1, (int)u->__last_used);

	/* Release by a non-holder: holder stays. */
	u->__holder = c1;
	usb__release_safe(u, c2);
	TEST_ASSERT_EQUAL_PTR(c1, usb__holder(u));

	/* cooldown 50: __last_used stamped with the current time. */
	u->__dongle_cooldown = 50;
	u->__last_used = -1;
	u->__holder = c1;
	usb__release_safe(u, c1);
	TEST_ASSERT_NULL(usb__holder(u));
	TEST_ASSERT_TRUE(u->__last_used >= 0);
	TEST_ASSERT_TRUE(u->__last_used <= get_time() + 10);

	coder__destroy(c2);
	coder__destroy(c1);
	usb__destroy(u);
}

static void test_usb_delete(void)
{
	t_config	cfg;
	t_usb		*u;
	t_coder		*c1;
	t_coder		*c2;
	t_coder		*c3;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);
	c3 = mk_coder(3, 30);

	usb__acquire(u, c1);
	usb__acquire(u, c2);
	usb__delete(u, c1);
	TEST_ASSERT_EQUAL_PTR(c2, usb__first(u));

	/* Delete an absent coder: no-op. */
	usb__delete(u, c3);
	TEST_ASSERT_EQUAL_PTR(c2, usb__first(u));

	coder__destroy(c3);
	coder__destroy(c2);
	coder__destroy(c1);
	usb__destroy(u);
}

static void test_usb_delete_safe(void)
{
	t_config	cfg;
	t_usb		*u;
	t_coder		*c1;
	t_coder		*c2;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	c1 = mk_coder(1, 10);
	c2 = mk_coder(2, 20);

	usb__acquire(u, c1);
	usb__acquire(u, c2);
	usb__delete_safe(u, c1);
	TEST_ASSERT_EQUAL_PTR(c2, usb__first(u));

	coder__destroy(c2);
	coder__destroy(c1);
	usb__destroy(u);
}

static void test_usb_stop_safe(void)
{
	t_config	cfg;
	t_usb		*u;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	TEST_ASSERT_TRUE(u->__active);

	usb__stop_safe(u);
	TEST_ASSERT_FALSE(u->__active);

	usb__destroy(u);
}

static void test_usb_create_destroy(void)
{
	t_config	cfg;
	t_usb		*u;
	t_usb		*null_u;

	cfg = mk_config(FIFO, 0);
	u = usb__create(&cfg);
	TEST_ASSERT_NOT_NULL(u);
	usb__destroy(u);

	/* NULL destroy must be a safe no-op. */
	null_u = NULL;
	usb__destroy(null_u);
}

void	run_usb_tests(void)
{
	RUN_TEST(test_usb_create_fifo);
	RUN_TEST(test_usb_create_edf);
	RUN_TEST(test_usb_create_unknown);
	RUN_TEST(test_usb_stack_init_reset);
	RUN_TEST(test_usb_cooldown_deadline);
	RUN_TEST(test_usb_acquire);
	RUN_TEST(test_usb_release_safe);
	RUN_TEST(test_usb_delete);
	RUN_TEST(test_usb_delete_safe);
	RUN_TEST(test_usb_stop_safe);
	RUN_TEST(test_usb_create_destroy);
}