/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_th.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:06:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/28 21:00:14 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"
#include "hub.h"

/*
* Sweeps every coder once against the same clock sample. Returns the first
* coder that burned out, or NULL when everyone is alive. Sets all_done when
* every coder met its compile quota, and stores on the monitor the smallest
* time left before the nearest burnout deadline.
*/
static t_coder *NULLABLE	monitor__th_scan(
	t_monitor *NONNULL self,
	t_ms now,
	bool *NONNULL all_done
)
{
	size_t	i;
	t_ms	left;

	*all_done = true;
	self->__min_left = -1;
	i = 0;
	while (i < self->__n_coders)
	{
		if (self->__coders[i]->__compiles
			< self->__coders[i]->__number_of_compiles_required)
		{
			*all_done = false;
			if (now - self->__coders[i]->__last_compile
				> self->__time_to_burnout)
				return (self->__coders[i]);
			left = self->__coders[i]->__last_compile
				+ self->__time_to_burnout - now;
			if (self->__min_left == -1 || left < self->__min_left)
				self->__min_left = left;
		}
		i++;
	}
	return (NULL);
}

/*
* The monitor is the only watcher: it decides when the simulation ends,
* either because someone burned out or because every coder met its quota.
* Both cases funnel through hub__end. Each sweep samples the clock once
* and sleeps toward the nearest deadline; within a tick of a deadline the
* sweep spins, so the burnout message goes out as fast as possible.
*/
static void	*monitor__th_start_routine(t_monitor *NONNULL self)
{
	t_coder	*burned;
	t_ms	now;
	bool	all_done;

	while (hub__is_running(self->__hub))
	{
		now = get_sim_time(false);
		burned = monitor__th_scan(self, now, &all_done);
		if (burned)
			logger__add_to_queue(self->__logger, burned->__id, BURNED);
		if (burned || all_done)
		{
			hub__end(self->__hub);
			return (NULL);
		}
		if (self->__min_left > MONITOR_SLEEP_CAP_MS)
			sleep_ms(MONITOR_SLEEP_CAP_MS);
		else if (self->__min_left > MONITOR_TICK_MS)
			sleep_ms(self->__min_left - MONITOR_TICK_MS);
	}
	return (NULL);
}

int	monitor__init_thread(t_monitor *NONNULL self)
{
	if (pthread_create(
			&self->___thread,
			NULL,
			(void *NULLABLE (*NONNULL)(void *NONNULL))monitor__th_start_routine,
		self
	)
	)
		return (print_error("FAILED CREATING MONITOR THREAD", false));
	self->__thread_active = true;
	return (0);
}

int	monitor__join_thread(t_monitor *NONNULL self)
{
	if (!self->__thread_active)
		return (0);
	self->__thread_active = false;
	return (pthread_join(self->___thread, NULL));
}
