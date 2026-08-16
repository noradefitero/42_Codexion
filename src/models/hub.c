/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:02:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 08:24:43 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

int	hub__init(t_hub *self, size_t n_coders)
{
	self->__n_coders = n_coders;
	self->__usbs = hub__create_usbs(n_coders);
	if (!self->__usbs)
		return (1);
	self->__coders = hub__create_coders(n_coders, self->__usbs);
	if (!self->__coders)
		return (1);
	return (0);
}

t_hub	*hub__create(size_t n_coders)
{
	t_hub	*result;

	result = (t_hub *)ft_calloc(1, sizeof(t_hub));
	if (!result)
		return (NULL);
	if (hub__init(result, n_coders))
	{
		hub__destroy(result);
		return (NULL);
	}
	return (result);
}

void	hub__reset(t_hub *self)
{
	if (self->__coders)
	{
		hub__destroy_coders(self->__coders, self->__n_coders);
		free(self->__coders);
	}
	if (self->__usbs)
	{
		hub__destroy_usbs(self->__usbs, self->__n_coders);
		free(self->__usbs);
	}
}

void	hub__destroy(t_hub *hub)
{
	if (hub)
	{
		hub__reset(hub);
		free(hub);
	}
}
