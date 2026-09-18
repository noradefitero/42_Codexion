/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_monitor.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:47:23 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:24 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"

#include "../../src/models/monitor.h"
#include "../../src/models/coder.h"
#include "../../src/models/hub.h"

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

static void	test_monitor_init(void)
{
	t_config	cfg = mk_cfg();
	t_coder		*coders[2];
	t_monitor	m;

	coders[0] = coder__create(1, &cfg, NULL, NULL);
	coders[1] = coder__create(2, &cfg, NULL, NULL);
	TEST_ASSERT_NOT_NULL(coders[0]);
	TEST_ASSERT_NOT_NULL(coders[1]);

	monitor__init(&m, coders, &cfg, NULL);
	TEST_ASSERT_EQUAL_PTR(coders, m.__coders);
	TEST_ASSERT_EQUAL_INT(2, (int)m.__n_coders);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_burnout, m.__time_to_burnout);
	TEST_ASSERT_NULL(m.__logger);
	TEST_ASSERT_FALSE(m.__thread_active);
	TEST_ASSERT_NULL(m.__hub);

	coder__destroy(coders[0]);
	coder__destroy(coders[1]);
}

static void	test_monitor_create_set_hub_destroy(void)
{
	t_config	cfg = mk_cfg();
	t_coder		*coders[2];
	t_hub		hub = {0};
	t_monitor	*m;

	coders[0] = coder__create(1, &cfg, NULL, NULL);
	coders[1] = coder__create(2, &cfg, NULL, NULL);
	TEST_ASSERT_NOT_NULL(coders[0]);
	TEST_ASSERT_NOT_NULL(coders[1]);

	m = monitor__create(coders, &cfg, NULL, NULL);
	TEST_ASSERT_NOT_NULL(m);
	TEST_ASSERT_NULL(m->__hub);

	monitor__set_hub(m, &hub);
	TEST_ASSERT_EQUAL_PTR(&hub, m->__hub);

	monitor__destroy(m);
	coder__destroy(coders[0]);
	coder__destroy(coders[1]);
}

static void	test_monitor_join_thread_inactive(void)
{
	t_config	cfg = mk_cfg();
	t_coder		*coders[2];
	t_monitor	m;

	coders[0] = coder__create(1, &cfg, NULL, NULL);
	coders[1] = coder__create(2, &cfg, NULL, NULL);

	monitor__init(&m, coders, &cfg, NULL);
	TEST_ASSERT_EQUAL_INT(0, monitor__join_thread(&m));
	TEST_ASSERT_FALSE(m.__thread_active);

	coder__destroy(coders[0]);
	coder__destroy(coders[1]);
}

static void	test_monitor_reset_fresh(void)
{
	t_config	cfg = mk_cfg();
	t_coder		*coders[2];
	t_monitor	m;

	coders[0] = coder__create(1, &cfg, NULL, NULL);
	coders[1] = coder__create(2, &cfg, NULL, NULL);

	monitor__init(&m, coders, &cfg, NULL);
	monitor__reset(&m); /* fresh monitor: must not crash */

	coder__destroy(coders[0]);
	coder__destroy(coders[1]);
}

void	run_monitor_tests(void)
{
	RUN_TEST(test_monitor_init);
	RUN_TEST(test_monitor_create_set_hub_destroy);
	RUN_TEST(test_monitor_join_thread_inactive);
	RUN_TEST(test_monitor_reset_fresh);
}