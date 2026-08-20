/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 08:30:52 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

int	coder__init(
	t_coder *NONNULL self,
	int id,
	t_config *NONNULL config,
	t_logger *NULLABLE logger
)
{
	self->__time_to_compile = config->time_to_compile;
	self->__time_to_debug = config->time_to_debug;
	self->__time_to_refactor = config->time_to_refactor;
	self->__number_of_compiles_required = config->number_of_compiles_required;
	self->__compiles = 0;
	self->__id = id;
	self->__state = COMPILE;
	self->__logger = logger;
	self->__exit_thread = false;
	self->__thread_active = false;
	return (0);
}

t_coder	*coder__create(
	int id,
	t_config *NULLABLE config,
	t_logger *NULLABLE logger
)
{
	t_coder	*result;

	result = (t_coder *)ft_calloc(1, sizeof(t_coder));
	if (!result)
		return (NULL);
	if (coder__init(result, id, config, logger))
	{
		coder__destroy(result);
		return (NULL);
	}
	return (result);
}

void	coder__reset(t_coder *self)
{
	if (self->__thread_active)
	{
		coder__exit_thread(self);
		coder__join_thread(self);
	}
}

void	coder__destroy(t_coder *coder)
{
	if (coder)
	{
		coder__reset(coder);
		free(coder);
	}
}
