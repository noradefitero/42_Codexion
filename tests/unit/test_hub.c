/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_hub.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:46:59 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:47:01 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Unity/src/unity.h"

#include "../../src/models/hub.h"
#include "../../src/models/coder.h"
#include "../../src/models/usb.h"
#include "../../src/models/logger.h"
#include "../../src/utils/time.h"
#include "capture.h"

#include <pthread.h>
#include <stdbool.h>
#include <string.h>

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

/* hub__coders_map callbacks */
static int	s_count_coders;

static int	count_coder(t_coder *coder)
{
	(void)coder;
	s_count_coders++;
	return (0);
}

static int	return_one_if_id_two(t_coder *coder)
{
	return (coder->__id == 2 ? 1 : 0);
}

/* threading helpers for the start/end gate tests */
typedef struct s_th_ctx
{
	t_hub	*hub;
	bool	done;
}	t_th_ctx;

static void	*th_wait_start_routine(void *arg)
{
	t_th_ctx	*ctx = (t_th_ctx *)arg;

	hub__wait_start(ctx->hub);
	ctx->done = true;
	return (NULL);
}

static void	*th_wait_end_routine(void *arg)
{
	t_th_ctx	*ctx = (t_th_ctx *)arg;

	hub__wait_end(ctx->hub);
	ctx->done = true;
	return (NULL);
}

static int	str_count(const char *haystack, const char *needle)
{
	const char	*p = haystack;
	const size_t	n = strlen(needle);
	int			count = 0;

	while ((p = strstr(p, needle)) != NULL)
	{
		count++;
		p += n;
	}
	return (count);
}

static void	test_hub_init_reset(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));

	/* config copied field by field */
	TEST_ASSERT_EQUAL_INT(cfg.number_of_coders, hub.__config.number_of_coders);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_burnout, hub.__config.time_to_burnout);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_compile, hub.__config.time_to_compile);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_debug, hub.__config.time_to_debug);
	TEST_ASSERT_EQUAL_INT(cfg.time_to_refactor, hub.__config.time_to_refactor);
	TEST_ASSERT_EQUAL_INT(
		cfg.number_of_compiles_required,
		hub.__config.number_of_compiles_required);
	TEST_ASSERT_EQUAL_INT(cfg.dongle_cooldown, hub.__config.dongle_cooldown);
	TEST_ASSERT_EQUAL_INT(cfg.scheduler, hub.__config.scheduler);

	/* N entries allocated in both arrays */
	TEST_ASSERT_NOT_NULL(hub.__coders);
	TEST_ASSERT_NOT_NULL(hub.__usbs);
	TEST_ASSERT_NOT_NULL(hub.__coders[0]);
	TEST_ASSERT_NOT_NULL(hub.__coders[1]);
	TEST_ASSERT_NOT_NULL(hub.__usbs[0]);
	TEST_ASSERT_NOT_NULL(hub.__usbs[1]);

	TEST_ASSERT_FALSE(hub__is_running(&hub));
	TEST_ASSERT_NOT_NULL(hub__monitor(&hub));
	TEST_ASSERT_NOT_NULL(hub__logger(&hub));
	TEST_ASSERT_TRUE(hub.__logger.__pool_size > 0);

	/* reset frees the arrays; a second reset and destroy(NULL) are safe */
	hub__reset(&hub);
	TEST_ASSERT_NULL(hub.__coders);
	TEST_ASSERT_NULL(hub.__usbs);
	hub__reset(&hub);
	hub__destroy(NULL);
}

static void	test_hub_wiring_n2(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	TEST_ASSERT_EQUAL_PTR(
		hub.__usbs[0], coder__left_usb(hub.__coders[0]));
	TEST_ASSERT_EQUAL_PTR(
		hub.__usbs[1], coder__right_usb(hub.__coders[0]));
	TEST_ASSERT_EQUAL_PTR(
		hub.__usbs[1], coder__left_usb(hub.__coders[1]));
	TEST_ASSERT_EQUAL_PTR(
		hub.__usbs[0], coder__right_usb(hub.__coders[1]));
	hub__reset(&hub);
}

static void	test_hub_wiring_n1(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	cfg.number_of_coders = 1;
	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	TEST_ASSERT_EQUAL_PTR(
		hub.__usbs[0], coder__left_usb(hub.__coders[0]));
	/* hub__create_coders only wires a right dongle when N > 1: a lone
	* coder has no neighbour, so coder__th_n1_wait_burn() takes over. */
	TEST_ASSERT_NULL(coder__right_usb(hub.__coders[0]));
	hub__reset(&hub);
}

static void	test_hub_create_destroy(void)
{
	t_config	cfg = mk_cfg();
	t_hub		*hub;

	hub = hub__create(&cfg);
	TEST_ASSERT_NOT_NULL(hub);
	hub__destroy(hub);
}

static void	test_hub_coders_map(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));

	s_count_coders = 0;
	TEST_ASSERT_EQUAL_INT(0, hub__coders_map(&hub, count_coder));
	TEST_ASSERT_EQUAL_INT(2, s_count_coders);

	TEST_ASSERT_EQUAL_INT(1, hub__coders_map(&hub, return_one_if_id_two));
	hub__reset(&hub);
}

static void	test_hub_end(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));

	/* not running → early return, usbs stay active */
	hub__end(&hub);
	TEST_ASSERT_FALSE(hub.__running);
	TEST_ASSERT_TRUE(hub.__usbs[0]->__active);
	TEST_ASSERT_TRUE(hub.__usbs[1]->__active);

	/* running → hub__end stops every usb */
	hub.__running = true;
	hub__end(&hub);
	TEST_ASSERT_FALSE(hub.__running);
	TEST_ASSERT_FALSE(hub.__usbs[0]->__active);
	TEST_ASSERT_FALSE(hub.__usbs[1]->__active);

	hub__reset(&hub);
}

static void	test_hub_wait_non_running(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	hub__wait_start(&hub); /* must return immediately */
	hub__wait_end(&hub); /* must return immediately */
	TEST_ASSERT_FALSE(hub.__running);
	hub__reset(&hub);
}

static void	test_hub_signal_start(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	pthread_t	th;
	t_th_ctx	ctx;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	hub.__running = true;
	ctx.hub = &hub;
	ctx.done = false;

	TEST_ASSERT_EQUAL_INT(
		0, pthread_create(&th, NULL, th_wait_start_routine, &ctx));
	sleep_ms(10);
	hub__signal_start(&hub);
	TEST_ASSERT_EQUAL_INT(0, pthread_join(th, NULL));

	TEST_ASSERT_TRUE(ctx.done);
	TEST_ASSERT_TRUE(hub.__coders[0]->__last_compile > 0);
	TEST_ASSERT_TRUE(hub.__coders[1]->__last_compile > 0);

	hub.__running = false;
	hub__reset(&hub);
}

static void	test_hub_wait_end_blocking(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	pthread_t	th;
	t_th_ctx	ctx;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	hub.__running = true;
	ctx.hub = &hub;
	ctx.done = false;

	TEST_ASSERT_EQUAL_INT(
		0, pthread_create(&th, NULL, th_wait_end_routine, &ctx));
	sleep_ms(20);
	hub__end(&hub);
	TEST_ASSERT_EQUAL_INT(0, pthread_join(th, NULL));

	TEST_ASSERT_TRUE(ctx.done);
	TEST_ASSERT_FALSE(hub.__running);
	hub__reset(&hub);
}

/*
* hub__run integration (a): a lone coder takes its single dongle and waits,
* so it burns out once time_to_burnout passes.
*/
static void	test_hub_run_n1_burns(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	t_capture	cap;
	t_ms		t0;
	char		*out;

	cfg.number_of_coders = 1;
	cfg.time_to_burnout = 150;
	cfg.time_to_compile = 30;
	cfg.time_to_debug = 20;
	cfg.time_to_refactor = 20;
	cfg.number_of_compiles_required = 2;
	cfg.dongle_cooldown = 0;
	cfg.scheduler = FIFO;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	t0 = get_time();
	/* flush Unity's pending output first: capture_end() flushes all streams
	* while stdout is still redirected, which would otherwise swallow the
	* PASS lines Unity buffers between tests. */
	fflush(stdout);
	TEST_ASSERT_EQUAL_INT(0, capture_begin(1, &cap));
	TEST_ASSERT_EQUAL_INT(0, hub__run(&hub));
	out = capture_end(&cap);

	TEST_ASSERT_TRUE(get_time() - t0 < 5000);
	TEST_ASSERT_TRUE(str_contains(out, "has taken a dongle"));
	TEST_ASSERT_TRUE(str_contains(out, "burned out"));
	TEST_ASSERT_EQUAL_INT(0, (int)hub.__logger.__size);

	free(out);
	hub__reset(&hub);
}

/*
* hub__run integration (b): two coders serialize on the shared dongle and
* both reach their compile quota long before the 800ms burnout.
*/
static void	test_hub_run_n2_fifo(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	t_capture	cap;
	t_ms		t0;
	char		*out;

	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 800;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = 2;
	cfg.dongle_cooldown = 0;
	cfg.scheduler = FIFO;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	t0 = get_time();
	fflush(stdout); /* flush Unity's pending output (see n1_burns case) */
	TEST_ASSERT_EQUAL_INT(0, capture_begin(1, &cap));
	TEST_ASSERT_EQUAL_INT(0, hub__run(&hub));
	out = capture_end(&cap);

	TEST_ASSERT_TRUE(get_time() - t0 < 5000);
	TEST_ASSERT_TRUE(str_count(out, "is compiling") >= 4);
	TEST_ASSERT_FALSE(str_contains(out, "burned out"));
	TEST_ASSERT_EQUAL_INT(2, hub.__coders[0]->__compiles);
	TEST_ASSERT_EQUAL_INT(2, hub.__coders[1]->__compiles);
	TEST_ASSERT_EQUAL_INT(0, (int)hub.__logger.__size);

	free(out);
	hub__reset(&hub);
}

/*
* hub__run integration (c): same as (b) under the EDF scheduler.
*/
static void	test_hub_run_n2_edf(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	t_capture	cap;
	t_ms		t0;
	char		*out;

	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 800;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = 2;
	cfg.dongle_cooldown = 0;
	cfg.scheduler = EDF;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	t0 = get_time();
	fflush(stdout); /* flush Unity's pending output (see n1_burns case) */
	TEST_ASSERT_EQUAL_INT(0, capture_begin(1, &cap));
	TEST_ASSERT_EQUAL_INT(0, hub__run(&hub));
	out = capture_end(&cap);

	TEST_ASSERT_TRUE(get_time() - t0 < 5000);
	TEST_ASSERT_TRUE(str_count(out, "is compiling") >= 4);
	TEST_ASSERT_FALSE(str_contains(out, "burned out"));
	TEST_ASSERT_EQUAL_INT(2, hub.__coders[0]->__compiles);
	TEST_ASSERT_EQUAL_INT(2, hub.__coders[1]->__compiles);
	TEST_ASSERT_EQUAL_INT(0, (int)hub.__logger.__size);

	free(out);
	hub__reset(&hub);
}

/*
* hub__run integration (d): the README burnout scenario. A 34ms compile
* exceeds the 20ms burnout, so nobody can finish the 20-compile quota and
* the monitor ends the simulation with a burnout. This run happens after
* test_hub_run_n1_burns, whose burnout latched log_state()'s static flag
* (not resettable from tests), so stdout assertions are unavailable here;
* the observable proof is that neither coder completed the quota.
*/
static void	test_hub_run_n2_burn_readme(void)
{
	t_config	cfg = mk_cfg();
	t_hub		hub;
	t_capture	cap;
	t_ms		t0;
	char		*out;

	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 20;
	cfg.time_to_compile = 34;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 34;
	cfg.number_of_compiles_required = 20;
	cfg.dongle_cooldown = 50;
	cfg.scheduler = FIFO;

	TEST_ASSERT_EQUAL_INT(0, hub__init(&hub, &cfg));
	t0 = get_time();
	fflush(stdout); /* flush Unity's pending output (see n1_burns case) */
	TEST_ASSERT_EQUAL_INT(0, capture_begin(1, &cap));
	TEST_ASSERT_EQUAL_INT(0, hub__run(&hub));
	out = capture_end(&cap);

	TEST_ASSERT_TRUE(get_time() - t0 < 5000);
	TEST_ASSERT_TRUE(hub.__coders[0]->__compiles < 20);
	TEST_ASSERT_TRUE(hub.__coders[1]->__compiles < 20);
	TEST_ASSERT_EQUAL_INT(0, (int)hub.__logger.__size);

	free(out);
	hub__reset(&hub);
}

void	run_hub_tests(void)
{
	RUN_TEST(test_hub_init_reset);
	RUN_TEST(test_hub_wiring_n2);
	RUN_TEST(test_hub_wiring_n1);
	RUN_TEST(test_hub_create_destroy);
	RUN_TEST(test_hub_coders_map);
	RUN_TEST(test_hub_end);
	RUN_TEST(test_hub_wait_non_running);
	RUN_TEST(test_hub_signal_start);
	RUN_TEST(test_hub_wait_end_blocking);
	/* non-burning runs first, then the burning ones (see file header) */
	RUN_TEST(test_hub_run_n2_fifo);
	RUN_TEST(test_hub_run_n2_edf);
	RUN_TEST(test_hub_run_n1_burns);
	RUN_TEST(test_hub_run_n2_burn_readme);
}