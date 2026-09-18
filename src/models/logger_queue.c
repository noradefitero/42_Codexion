/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 01:10:58 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/15 22:16:32 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

/*
* Grows the pool and re-linearizes the ring: the logical window
* [head .. head + size) is copied back to the start of the new buffer,
* because ft_realloc cannot move a wrapped circular buffer in place.
*/
static int	logger__increase_pool(t_logger *NONNULL self)
{
	const size_t	old_size = self->__pool_size;
	const size_t	new_size = old_size + old_size / 2;
	const size_t	first = old_size - self->__head;
	t_log			*new;

	new = (t_log *)malloc(new_size * sizeof(t_log));
	if (!new)
		return (-1);
	ft_memcpy(new, self->__queue + self->__head, first * sizeof(t_log));
	ft_memcpy(new + first, self->__queue, self->__head * sizeof(t_log));
	free(self->__queue);
	self->__queue = new;
	self->__pool_size = new_size;
	self->__head = 0;
	self->__tail = self->__size;
	return (0);
}

/*
* The timestamp is sampled before taking the mutex so the critical
* section stays as short as possible. The logger is woken up only on the
* 0 -> 1 transition: once it is draining, it re-checks the queue size
* under the mutex and needs no further wakeups.
*/
void	logger__add_to_queue(
	t_logger *NONNULL self,
	int coder_id,
	t_log_mess state
)
{
	t_ms			now;
	t_log *NONNULL	log;

	now = get_sim_time(false);
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
	log->timestamp = now;
	log->coder_id = coder_id;
	log->state = state;
	self->__tail = (self->__tail + 1) % self->__pool_size;
	self->__size++;
	if (self->__size == 1)
		pthread_cond_broadcast(&self->__cond);
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
