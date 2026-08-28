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

static bool	monitor__th_check_if_burned(t_coder *coder, int burning_time)
{
	if (coder->__compiles >= coder->__number_of_compiles_required)
		return (false);
	return (get_sim_time(false) - coder->__last_compile > burning_time);
}

static bool	monitor__th_all_done(t_monitor *NONNULL self)
{
	size_t	i;

	i = 0;
	while (i < self->__n_coders)
	{
		if (self->__coders[i]->__compiles
			< self->__coders[i]->__number_of_compiles_required)
			return (false);
		i++;
	}
	return (true);
}

/*
* The monitor is the only watcher: it decides when the simulation ends, either
* because someone burned out or because every coder met its quota. Both cases
* funnel through hub__end.
*/
static void	*monitor__th_start_routine(t_monitor *NONNULL self)
{
	size_t	i;

	while (hub__is_running(self->__hub))
	{
		i = 0;
		while (i < self->__n_coders && hub__is_running(self->__hub))
		{
			if (monitor__th_check_if_burned(
					self->__coders[i], self->__time_to_burnout))
			{
				logger__add_to_queue(
					self->__logger, self->__coders[i]->__id, BURNED);
				hub__end(self->__hub);
				return (NULL);
			}
			i++;
		}
		if (monitor__th_all_done(self))
		{
			hub__end(self->__hub);
			return (NULL);
		}
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
