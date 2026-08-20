/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_th.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:06:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/19 02:06:52 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

static void	*logger__th_start_routine(t_logger *NONNULL self)
{
	t_log	*log;

	while (!self->__exit_flag)
	{
		log = logger__pop_queue(self);
		if (log)
		{
			log_state(log->coder_id, log->timestamp, log->state);
			free (log);
		}
	}
	self->__thread_active = false;
	return (NULL);
}

int	logger__init_thread(t_logger *NONNULL self)
{
	if (pthread_create(
			&self->___thread,
			NULL,
			(void *NULLABLE (*NONNULL)(void *NONNULL))logger__th_start_routine,
		self
	)
	)
		return (1);
	self->__thread_active = true;
	return (0);
}

int	logger__join_thread(t_logger *NONNULL self)
{
	return (pthread_join(self->___thread, NULL));
}

int	logger__exit_thread(t_logger *NONNULL self)
{
	if (self->__thread_active)
	{
		self->__exit_flag = true;
		return (logger__join_thread(self));
	}
	return (0);
}
