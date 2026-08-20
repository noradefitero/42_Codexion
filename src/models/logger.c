/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:00:59 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 08:36:34 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

int	logger__init(t_logger *NONNULL self)
{
	self->__queue = NULL;
	self->__queue_tail = NULL;
	self->__thread_active = false;
	self->__exit_flag = false;
	self->__queue_active = true;
	self->__cond_initialized = false;
	self->__mutex_initialized = false;
	if (pthread_mutex_init(&self->__mutex, NULL))
	{
		logger__reset(self);
		return (1);
	}
	self->__mutex_initialized = true;
	if (pthread_cond_init(&self->__cond, NULL))
	{
		logger__reset(self);
		return (1);
	}
	self->__cond_initialized = true;
	return (0);
}

t_logger *NULLABLE	logger__create(void)
{
	t_logger	*result;

	result = (t_logger *)ft_calloc(1, sizeof(t_logger));
	if (!result)
		return (NULL);
	if (logger__init(result))
	{
		logger__destroy(result);
		return (NULL);
	}
	return (result);
}

void	logger__reset(t_logger *NONNULL self)
{
	if (self->__thread_active)
		logger__exit_thread(self);
	if (self->__mutex_initialized)
	{
		pthread_mutex_destroy(&self->__mutex);
		self->__mutex_initialized = false;
	}
	if (self->__cond_initialized)
	{
		pthread_cond_destroy(&self->__cond);
		self->__cond_initialized = false;
	}
	if (self->__queue)
		logger__clear_queue(self);
}

void	logger__destroy(t_logger *NONNULL logger)
{
	if (logger)
	{
		logger__reset(logger);
		free(logger);
	}
}
