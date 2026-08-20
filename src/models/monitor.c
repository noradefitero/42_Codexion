/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:00:59 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/18 09:15:51 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"

void	monitor__init(
		t_monitor *NONNULL self,
		t_coder *NONNULL *NONNULL coders,
		t_config *NONNULL config,
		t_hub *NULLABLE hub
)
{
	self->__thread_active = false;
	self->__exit_thread = false;
	self->__coders = coders;
	self->__n_coders = config->number_of_coders;
	self->__time_to_burnout = config->time_to_burnout;
	self->__hub = hub;
}

t_monitor *NULLABLE	monitor__create(
	t_coder *NONNULL *NONNULL coders,
	t_config *NONNULL config,
	t_hub *NULLABLE hub
)
{
	t_monitor	*result;

	result = (t_monitor *)ft_calloc(1, sizeof(t_monitor));
	if (!result)
		return (NULL);
	monitor__init(result, coders, config, hub);
	return (result);
}

void	monitor__reset(t_monitor *NONNULL self)
{
	if (self->__thread_active)
		monitor__exit_thread(self);
}

void	monitor__destroy(t_monitor *NONNULL monitor)
{
	if (monitor)
	{
		monitor__reset(monitor);
		free(monitor);
	}
}
