/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:02:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 08:18:45 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

int	hub__init(t_hub *NONNULL self, t_config *NONNULL config)
{
	self->__config = *config;
	self->__coders = NULL;
	self->__usbs = NULL;
	self->__logger_init = false;
	self->__monitor_init = false;
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
	monitor__init(
		&self->__monitor, self->__coders, &self->__config, self);
	self->__monitor_init = true;
	if (logger__init(&self->__logger, self->__config.number_of_coders))
	{
		hub__reset(self);
		return (-1);
	}
	self->__logger_init = true;
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
	monitor__reset(&self->__monitor);
	monitor__join_thread(&self->__monitor);
	logger__reset(&self->__logger);
	logger__join_thread(&self->__logger);
	if (self->__usbs)
	{
		hub__destroy_usbs(self->__usbs, self->__config.number_of_coders);
		free(self->__usbs);
		self->__coders = NULL;
	}
	if (self->__coders)
	{
		hub__destroy_coders(self->__coders, self->__config.number_of_coders);
		free(self->__coders);
		self->__coders = NULL;
	}
}

void	hub__destroy(t_hub *NONNULL hub)
{
	if (hub)
	{
		hub__reset(hub);
		free(hub);
	}
}
