/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_coder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:46:08 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:46:21 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"

#include "../../src/models/coder.h"
#include "../../src/models/hub.h"
#include "../../src/models/usb.h"
#include "../../src/models/logger.h"
#include "../../src/utils/time.h"

/*
* Unity runs setUp/tearDown around every test in this binary. This suite
* has no shared fixture, so both stay empty.
*/
void	setUp(void)
{
}

void	tearDown(void)
{
}

/*
* Valid baseline config: 2 coders, every time field 10ms, two compiles
* required, no dongle cooldown, FIFO scheduling.
*/
static t_config	mk_cfg(void)
{
	t_config	cfg;

	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 10;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = 2;
	cfg.dongle_cooldown = 0;
	cfg.scheduler = FIFO;
	return (cfg);
}

static void	test_coder_init(void)
{
	t_config	cfg = mk_cfg();
	t_coder		c;

	coder__init(&c, 5, &cfg, NULL);

	/* all time fields copied from config */
	TEST_ASSERT_EQUAL_INT(cfg.time_to_compile, c.__time_to_compile);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_debug, c.__time_to_debug);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_refactor, c.__time_to_refactor);
	TEST_ASSERT_EQUAL_INT(
		cfg.number_of_compiles_required, c.__number_of_compiles_required);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_burnout, c.__time_to_burnout);
	/* fresh lifecycle state */
	TEST_ASSERT_EQUAL_INT(0, c.__compiles);
	TEST_ASSERT_EQUAL_INT(COMPILE, c.__state);
	TEST_ASSERT_EQUAL_INT(5, c.__id);
	TEST_ASSERT_NULL(c.__hub);
	TEST_ASSERT_NULL(c.__logger);
	TEST_ASSERT_NULL(c.__left_usb);
	TEST_ASSERT_NULL(c.__right_usb);
	TEST_ASSERT_FALSE(c.__thread_active);
}

static void	test_coder_create(void)
{
	t_config	cfg = mk_cfg();
	t_logger	logger;
	t_hub		hub;
	t_coder		*c;

	TEST_ASSERT_EQUAL_INT(0, logger__init(&logger, 2, 2));
	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));

	c = coder__create(3, &cfg, &logger, &hub);
	TEST_ASSERT_NOT_NULL(c);
	TEST_ASSERT_EQUAL_PTR(&hub, c->__hub);
	TEST_ASSERT_EQUAL_PTR(&logger, c->__logger);

	coder__destroy(c);
	hub__reset(&hub);
	logger__reset(&logger);
}

static void	test_coder_deadline(void)
{
	t_config	cfg = mk_cfg();
	t_coder		c;

	coder__init(&c, 1, &cfg, NULL);
	TEST_ASSERT_EQUAL_INT64(
		c.__last_compile + c.__time_to_burnout, coder__deadline(&c));

	c.__last_compile = 50;
	TEST_ASSERT_EQUAL_INT64(
		50 + c.__time_to_burnout, coder__deadline(&c));
}

static void	test_coder_setters_usbs(void)
{
	t_config	cfg = mk_cfg();
	t_usb		*left;
	t_usb		*right;
	t_coder		c;

	left = usb__create(&cfg);
	right = usb__create(&cfg);
	TEST_ASSERT_NOT_NULL(left);
	TEST_ASSERT_NOT_NULL(right);

	coder__init(&c, 1, &cfg, NULL);
	coder__set_left_usb(&c, left);
	coder__set_right_usb(&c, right);

	TEST_ASSERT_EQUAL_PTR(left, coder__left_usb(&c));
	TEST_ASSERT_EQUAL_PTR(right, coder__right_usb(&c));

	usb__destroy(left);
	usb__destroy(right);
}

static void	test_coder_set_hub(void)
{
	t_config	cfg = mk_cfg();
	t_hub		dummy = {0};
	t_coder		c;

	coder__init(&c, 1, &cfg, NULL);
	coder__set_hub(&c, &dummy);
	TEST_ASSERT_EQUAL_PTR(&dummy, c.__hub);
	coder__set_hub(&c, NULL);
	TEST_ASSERT_NULL(c.__hub);
}

static void	test_coder_reset(void)
{
	t_config	cfg = mk_cfg();
	t_coder		c;

	coder__init(&c, 1, &cfg, NULL);
	c.__state = REFACTOR;
	c.__compiles = 7;

	coder__reset(&c);
	TEST_ASSERT_EQUAL_INT(COMPILE, c.__state);
	TEST_ASSERT_EQUAL_INT(0, c.__compiles);
}

static void	test_coder_join_thread_inactive(void)
{
	t_config	cfg = mk_cfg();
	t_coder		c;

	coder__init(&c, 1, &cfg, NULL);
	TEST_ASSERT_EQUAL_INT(0, coder__join_thread(&c));
	TEST_ASSERT_FALSE(c.__thread_active);
}

static void	test_coder_destroy(void)
{
	t_config	cfg = mk_cfg();
	t_coder		*c;

	c = coder__create(1, &cfg, NULL, NULL);
	TEST_ASSERT_NOT_NULL(c);
	coder__destroy(c); /* ASan validates the free */
}

/*
* coder__th_own_usb called straight from the test thread: no contention,
* so the dongle queue hands over immediately.
*/
static void	test_coder_th_own_usb(void)
{
	t_config	cfg = mk_cfg();
	t_config	cfg_cd = mk_cfg();
	t_usb		*usb;
	t_usb		*usb_cd;
	t_usb		*usb_stopped;
	t_logger	*logger;
	t_coder		*coder;
	t_log		entry;
	t_ms		t0;

	cfg.dongle_cooldown = 0;
	cfg_cd.dongle_cooldown = 20;

	logger = logger__create(1, 1);
	TEST_ASSERT_NOT_NULL(logger);
	coder = coder__create(1, &cfg, logger, NULL);
	TEST_ASSERT_NOT_NULL(coder);

	/* normal path (cooldown 0): own → holder + a TAKEN_DONGLE log */
	usb = usb__create(&cfg);
	TEST_ASSERT_EQUAL_INT(0, coder__th_own_usb(coder, usb));
	TEST_ASSERT_EQUAL_PTR(coder, usb__holder(usb));
	TEST_ASSERT_EQUAL_INT(1, (int)logger->__size);
	TEST_ASSERT_EQUAL_INT(1, logger__pop_queue(logger, &entry));
	TEST_ASSERT_EQUAL_INT(TAKEN_DONGLE, entry.state);
	TEST_ASSERT_EQUAL_INT(1, entry.coder_id);

	/* release → holder cleared; own again still succeeds */
	usb__release_safe(usb, coder);
	TEST_ASSERT_NULL(usb__holder(usb));
	TEST_ASSERT_EQUAL_INT(0, coder__th_own_usb(coder, usb));
	usb__destroy(usb);

	/* cooldown path: a fresh dongle (__last_used == -1) makes the first
	* own instant; releasing stamps __last_used and the second own has to
	* wait out the 20ms cooldown. */
	usb_cd = usb__create(&cfg_cd);
	TEST_ASSERT_EQUAL_INT(0, coder__th_own_usb(coder, usb_cd));
	usb__release_safe(usb_cd, coder);
	t0 = get_time();
	TEST_ASSERT_EQUAL_INT(0, coder__th_own_usb(coder, usb_cd));
	TEST_ASSERT_TRUE(get_time() - t0 >= 15);
	TEST_ASSERT_EQUAL_PTR(coder, usb__holder(usb_cd));
	usb__destroy(usb_cd);

	/* stopped dongle: acquire fails, nobody holds it */
	usb_stopped = usb__create(&cfg);
	usb__stop_safe(usb_stopped);
	TEST_ASSERT_EQUAL_INT(-1, coder__th_own_usb(coder, usb_stopped));
	TEST_ASSERT_NULL(usb__holder(usb_stopped));
	usb__destroy(usb_stopped);

	coder__destroy(coder);
	logger__destroy(logger);
}

void	run_coder_tests(void)
{
	RUN_TEST(test_coder_init);
	RUN_TEST(test_coder_create);
	RUN_TEST(test_coder_deadline);
	RUN_TEST(test_coder_setters_usbs);
	RUN_TEST(test_coder_set_hub);
	RUN_TEST(test_coder_reset);
	RUN_TEST(test_coder_join_thread_inactive);
	RUN_TEST(test_coder_destroy);
	RUN_TEST(test_coder_th_own_usb);
}