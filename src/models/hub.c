/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:02:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 07:27:21 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

int	hub__init(t_hub *NONNULL self, t_config *NONNULL config)
{
	self->__config = *config;
	self->__usbs = hub__create_usbs(&self->__config);
	self->__initial_time = -1;
	if (hub__create_usbs(self))
	{
		hub__reset(self);
		return (1);
	}
	if (hub__create_coders(self))
	{
		hub__reset(self);
		return (1);
	}
	return (0);
}

t_hub	*hub__create(t_config *NONNULL config)
{
	t_hub	*result;

	result = (t_hub *)ft_calloc(1, sizeof(t_hub));
	if (!result)
		return (NULL);
	if (hub__init(result, config))
	{
		hub__destroy(result);
		return (NULL);
	}
	return (result);
}

void	hub__reset(t_hub *NONNULL self)
{
	if (self->__coders)
	{
		hub__destroy_coders(self->__coders, self->__config.number_of_coders);
		free(self->__coders);
		self->__coders = NULL;
	}
	if (self->__usbs)
	{
		hub__destroy_usbs(self->__usbs, self->__config.number_of_coders);
		free(self->__usbs);
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
