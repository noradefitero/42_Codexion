/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:14:17 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

void	coder__init(
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
	self->__time_to_burnout = config->time_to_burnout;
	self->__compiles = 0;
	self->__last_compile = get_sim_time(false);
	self->__id = id;
	self->__state = COMPILE;
	self->__hub = NULL;
	self->__logger = logger;
	self->__left_usb = NULL;
	self->__right_usb = NULL;
	self->__thread_active = false;
}

t_coder	*coder__create(
	int id,
	t_config *NULLABLE config,
	t_logger *NULLABLE logger,
	t_hub *NULLABLE hub
)
{
	t_coder	*result;

	result = (t_coder *)ft_calloc(1, sizeof(t_coder));
	if (!result)
	{
		print_error("FAILED ALLOCATING A CODER INSTANCE", false);
		return (NULL);
	}
	coder__init(result, id, config, logger);
	coder__set_hub(result, hub);
	return (result);
}

/*
* Safe to call at any moment: the join is a no-op when the thread was never
* started or was already joined. Releasing resources happens in hub__reset
* once every thread is guaranteed to be dead.
*/
void	coder__reset(t_coder *NONNULL self)
{
	coder__join_thread(self);
	self->__thread_active = false;
	self->__state = COMPILE;
	self->__compiles = 0;
}

void	coder__destroy(t_coder *NULLABLE coder)
{
	if (coder)
	{
		coder__reset(coder);
		free(coder);
	}
}
