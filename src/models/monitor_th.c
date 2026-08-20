/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_th.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:06:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 06:36:41 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"
#include "hub.h"

static bool	monitor__th_check_if_burned(t_coder *coder, int burning_time)
{
	return (get_sim_time(false) - coder->__last_compile > burning_time);
}

static void	*monitor__th_start_routine(t_monitor *NONNULL self)
{
	size_t	i;

	while (*self->__coders && !self->__exit_thread)
	{
		i = 0;
		while (i < self->__n_coders && !self->__exit_thread)
		{
			if (monitor__th_check_if_burned(
					self->__coders[i], self->__time_to_burnout)
			)
			{
				log_state(i + 1, get_sim_time(false), BURNED);
				if (self->__hub)
					hub__on_burn(self->__hub);
				return (NULL);
			}
			i++;
		}
	}
	self->__thread_active = false;
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
		return (1);
	self->__thread_active = true;
	return (0);
}

int	monitor__join_thread(t_monitor *NONNULL self)
{
	return (pthread_join(self->___thread, NULL));
}

int	monitor__exit_thread(t_monitor *NONNULL self)
{
	if (self->__thread_active)
	{
		self->__exit_thread = true;
		return (monitor__join_thread(self));
	}
	return (0);
}
