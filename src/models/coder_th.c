/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 07:40:21 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

static inline void	coder__th_compile(t_coder *NONNULL self)
{
	/* TODO: Taking USB dongles logic */
	logger__add_to_queue(self->__logger, self->__id, COMPILING);
	usleep(ms_to_useconds(self->__time_to_compile));
	self->__last_compile = get_sim_time(false);
	self->__compiles++;
	self->__state = DEBUG;
}

static inline void	coder__th_debug(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, DEBUGGING);
	usleep(ms_to_useconds(self->__time_to_debug));
	self->__state = REFACTOR;
}

static inline void	coder__th_refactor(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, REFACTORING);
	usleep(ms_to_useconds(self->__time_to_refactor));
	self->__state = COMPILE;
}

void *NULLABLE	coder__th_start_routine(t_coder *NONNULL self)
{
	self->__last_compile = get_sim_time(false);
	while (!self->__exit_thread
		&& self->__compiles < self->__number_of_compiles_required)
	{
		if (self->__state == COMPILE)
			coder__th_compile(self);
		else if (self->__state == DEBUG)
			coder__th_debug(self);
		else if (self->__state == REFACTOR)
			coder__th_refactor(self);
	}
	self->__thread_active = false;
	return (NULL);
}
