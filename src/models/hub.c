/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:02:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:55:40 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

static int	hub__init_core(t_hub *NONNULL self)
{
	if (hub__create_usbs(self))
	{
		hub__reset(self);
		return (-1);
	}
	if (hub__create_coders(self))
	{
		hub__reset(self);
		return (-1);
	}
	if (logger__init(
			&self->__logger,
			self->__config.number_of_coders,
			self->__config.number_of_compiles_required
		))
	{
		hub__reset(self);
		return (-1);
	}
	return (0);
}

int	hub__init(t_hub *NONNULL self, t_config *NONNULL config)
{
	memset(self, 0, sizeof(t_hub));
	self->__config = *config;
	get_sim_time(true);
	if (pthread_mutex_init(&self->__sim_mutex, NULL))
	{
		hub__reset(self);
		return (print_error("FAILED INITIALIZING SIM MUTEX", false));
	}
	self->__sim_mutex_init = true;
	if (pthread_cond_init(&self->__sim_cond, NULL))
	{
		hub__reset(self);
		return (print_error("FAILED INITIALIZING SIM COND", false));
	}
	self->__sim_cond_init = true;
	if (hub__init_core(self))
		return (-1);
	monitor__init(
		&self->__monitor,
		self->__coders,
		&self->__config,
		&self->__logger
		);
	monitor__set_hub(&self->__monitor, self);
	return (0);
}

t_hub	*hub__create(t_config *NONNULL config)
{
	t_hub	*result;

	result = (t_hub *)ft_calloc(1, sizeof(t_hub));
	if (!result)
	{
		print_error("FAILED ALLOCATING A HUB INSTANCE", false);
		return (NULL);
	}
	if (hub__init(result, config))
	{
		hub__destroy(result);
		return (NULL);
	}
	return (result);
}

void	hub__reset(t_hub *NONNULL self)
{
	if (self->__usbs)
	{
		hub__destroy_usbs(self->__usbs, self->__config.number_of_coders);
		free(self->__usbs);
		self->__usbs = NULL;
	}
	if (self->__coders)
	{
		hub__destroy_coders(self->__coders, self->__config.number_of_coders);
		free(self->__coders);
		self->__coders = NULL;
	}
	monitor__reset(&self->__monitor);
	logger__reset(&self->__logger);
	if (self->__sim_mutex_init)
	{
		self->__sim_mutex_init = false;
		pthread_mutex_destroy(&self->__sim_mutex);
	}
	if (self->__sim_cond_init)
	{
		self->__sim_cond_init = false;
		pthread_cond_destroy(&self->__sim_cond);
	}
	self->__running = false;
}

void	hub__destroy(t_hub *NULLABLE hub)
{
	if (hub)
	{
		hub__reset(hub);
		free(hub);
	}
}
