/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 01:10:58 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:48:41 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

static int	logger__increase_pool(t_logger *NONNULL self)
{
	const size_t	old_size = self->__pool_size;
	const size_t	new_size = old_size * LOG_POOL_REALLOC_MULTIPLIER;
	t_log			*new;

	new = ft_realloc(
			self->__queue,
			old_size * sizeof(t_log),
			new_size * sizeof(t_log)
			);
	if (!new)
		return (-1);
	self->__queue = new;
	self->__pool_size = new_size;
	self->__tail = (self->__head + self->__size) % new_size;
	return (0);
}

void	logger__add_to_queue(
	t_logger *NONNULL self,
	int coder_id,
	t_log_mess state
)
{
	t_log *NONNULL	log;

	pthread_mutex_lock(&self->__mutex);
	if (!self->__queue_init)
	{
		pthread_mutex_unlock(&self->__mutex);
		return ;
	}
	if (self->__size == self->__pool_size && logger__increase_pool(self))
	{
		print_error("LOGGER PRINTING POOL OVERFLOWED", false);
		pthread_mutex_unlock(&self->__mutex);
		return ;
	}
	log = &self->__queue[self->__tail];
	log->timestamp = get_sim_time(false);
	log->coder_id = coder_id;
	log->state = state;
	self->__tail = (self->__tail + 1) % self->__pool_size;
	self->__size++;
	pthread_cond_signal(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

/*
* Blocks until a log is available and copies it to buf (returns 1), or until
* the queue is empty and a stop was requested (returns 0).
*/
int	logger__pop_queue(t_logger *NONNULL self, t_log *NONNULL buf)
{
	pthread_mutex_lock(&self->__mutex);
	while (self->__queue_init && self->__size == 0 && !self->__exit_flag)
		pthread_cond_wait(&self->__cond, &self->__mutex);
	if (!self->__queue_init || self->__size == 0)
	{
		pthread_mutex_unlock(&self->__mutex);
		return (0);
	}
	*buf = self->__queue[self->__head];
	self->__head = (self->__head + 1) % self->__pool_size;
	self->__size--;
	pthread_mutex_unlock(&self->__mutex);
	return (1);
}

void	logger__wake(t_logger *NONNULL self)
{
	pthread_mutex_lock(&self->__mutex);
	if (self->__cond_initialized)
		pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

/*
* Asks the thread to exit once the queue is drained. Callers must still join
* the thread afterwards (hub__join_threads does).
*/
void	logger__request_stop(t_logger *NONNULL self)
{
	pthread_mutex_lock(&self->__mutex);
	self->__exit_flag = true;
	if (self->__cond_initialized)
		pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}
