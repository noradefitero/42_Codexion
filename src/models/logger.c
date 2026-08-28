/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:00:59 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:47:53 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

int	logger__init(t_logger *NONNULL self, size_t n_coders)
{
	self->___thread = (pthread_t)0;
	self->__thread_active = false;
	self->__exit_flag = false;
	self->__head = 0;
	self->__tail = 0;
	self->__size = 0;
	self->__pool_size = n_coders * LOG_POOL_MULTIPLIER;
	self->__queue_init = false;
	self->__cond_initialized = false;
	self->__mutex_initialized = false;
	self->__queue = malloc(sizeof(t_log) * self->__pool_size);
	if (!self->__queue)
	{
		logger__reset(self);
		return (print_error("FAILED ALLOCATING LOGGER POOL", false));
	}
	self->__queue_init = true;
	if (pthread_mutex_init(&self->__mutex, NULL))
	{
		logger__reset(self);
		return (print_error("FAILED INITIALIZING LOGGER MUTEX", false));
	}
	self->__mutex_initialized = true;
	if (pthread_cond_init(&self->__cond, NULL))
	{
		logger__reset(self);
		return (print_error("FAILED INITIALIZING LOGGER COND", false));
	}
	self->__cond_initialized = true;
	return (0);
}

t_logger *NULLABLE	logger__create(size_t n_coders)
{
	t_logger	*result;

	result = (t_logger *)ft_calloc(1, sizeof(t_logger));
	if (!result)
	{
		print_error("FAILED ALLOCATING A LOGGER INSTANCE", false);
		return (NULL);
	}
	if (logger__init(result, n_coders))
	{
		logger__destroy(result);
		return (NULL);
	}
	return (result);
}

/*
* Pure resource teardown: only legal once the thread has been joined
* (hub__run guarantees it). Safe on a zeroed logger.
*/
void	logger__reset(t_logger *NONNULL self)
{
	if (self->__queue_init)
	{
		self->__queue_init = false;
		free(self->__queue);
		self->__queue = NULL;
	}
	if (self->__mutex_initialized)
	{
		self->__mutex_initialized = false;
		pthread_mutex_destroy(&self->__mutex);
	}
	if (self->__cond_initialized)
	{
		self->__cond_initialized = false;
		pthread_cond_destroy(&self->__cond);
	}
}

void	logger__destroy(t_logger *NULLABLE logger)
{
	if (logger)
	{
		logger__reset(logger);
		free(logger);
	}
}
