/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 01:10:58 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 09:13:13 by dde-fite         ###   ########.fr       */
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

	if (!self->__queue_init)
		return ;
	pthread_mutex_lock(&self->__mutex);
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

int	logger__pop_queue(t_logger *NONNULL self, t_log *NONNULL buf)
{
	t_log *NONNULL	log;

	if (!self->__queue_init)
		return (0);
	pthread_mutex_lock(&self->__mutex);
	while (self->__size == 0)
	{
		if (self->__exit_flag)
		{
			pthread_mutex_unlock(&self->__mutex);
			return (0);
		}
		pthread_cond_wait(&self->__cond, &self->__mutex);
	}
	log = &self->__queue[self->__head];
	self->__head = (self->__head + 1) % self->__pool_size;
	self->__size--;
	*buf = *log;
	pthread_mutex_unlock(&self->__mutex);
	return (1);
}
