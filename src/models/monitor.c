/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:00:59 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:14:12 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"

void	monitor__init(
	t_monitor *NONNULL self,
	t_coder *NONNULL *NONNULL coders,
	t_config *NONNULL config,
	t_logger *NULLABLE logger
)
{
	self->___thread = (pthread_t)0;
	self->__thread_active = false;
	self->__coders = coders;
	self->__n_coders = config->number_of_coders;
	self->__time_to_burnout = config->time_to_burnout;
	self->__logger = logger;
	self->__hub = NULL;
}

t_monitor *NULLABLE	monitor__create(
	t_coder *NONNULL *NONNULL coders,
	t_config *NONNULL config,
	t_logger *NULLABLE logger,
	t_hub *NULLABLE hub
)
{
	t_monitor	*result;

	result = (t_monitor *)ft_calloc(1, sizeof(t_monitor));
	if (!result)
	{
		print_error("FAILED ALLOCATING A MONITOR INSTANCE", false);
		return (NULL);
	}
	monitor__init(result, coders, config, logger);
	monitor__set_hub(result, hub);
	return (result);
}

/*
* Safe to call at any moment: the join is a no-op when the thread was never
* started or was already joined.
*/
void	monitor__reset(t_monitor *NONNULL self)
{
	monitor__join_thread(self);
}

void	monitor__destroy(t_monitor *NULLABLE monitor)
{
	if (monitor)
	{
		monitor__reset(monitor);
		free(monitor);
	}
}

void	monitor__set_hub(t_monitor *NONNULL self, t_hub *NULLABLE hub)
{
	self->__hub = hub;
}
