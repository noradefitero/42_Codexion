/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 01:10:58 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 07:30:06 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

static inline t_log *NULLABLE	logger__new_log(int coder_id, t_log_mess state)
{
	t_log	*log;

	log = malloc(sizeof(t_log));
	if (!log)
	{
		print_error("FAILED ALLOCATING MEMORY FOR A LOG IN QUEUE", false);
		return (NULL);
	}
	log->timestamp = get_sim_time(false);
	log->coder_id = coder_id;
	log->state = state;
	log->next = NULL;
	return (log);
}

int	logger__add_to_queue(
	t_logger *NONNULL self,
	int coder_id,
	t_log_mess state
)
{
	t_log *NULLABLE	new;

	if (!self->__queue_active)
		return (1);
	new = logger__new_log(coder_id, state);
	if (!new)
		return (1);
	pthread_mutex_lock(&self->__mutex);
	if (self->__queue_tail)
		self->__queue_tail->next = new;
	else
		self->__queue = new;
	self->__queue_tail = new;
	pthread_cond_signal(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
	return (0);
}

t_log *NULLABLE	logger__pop_queue(t_logger *NONNULL self)
{
	t_log	*ret;

	if (!self->__queue_active)
		return (NULL);
	pthread_mutex_lock(&self->__mutex);
	while (!self->__queue)
	{
		if (!self->__queue_active || self->__exit_flag)
		{
			pthread_mutex_unlock(&self->__mutex);
			return (NULL);
		}
		pthread_cond_wait(&self->__cond, &self->__mutex);
	}
	ret = self->__queue;
	self->__queue = ret->next;
	if (!self->__queue)
		self->__queue_tail = NULL;
	ret->next = NULL;
	pthread_mutex_unlock(&self->__mutex);
	return (ret);
}

void	logger__clear_queue(t_logger *NONNULL self)
{
	t_log	*current;
	t_log	*next;

	self->__queue_active = false;
	pthread_mutex_lock(&self->__mutex);
	current = self->__queue;
	self->__queue = NULL;
	self->__queue_tail = NULL;
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
}
