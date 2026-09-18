/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_th.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 00:06:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:14:50 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.h"

/*
* Drains the queue until it is empty AND a stop was requested, so every log
* produced while the simulation was running gets printed before exiting.
* Timestamps are clamped to the previous printed one: producers sample the
* clock before taking the queue mutex, so entries can arrive slightly out
* of chronological order and the printed stream must stay monotonic.
*/
static void	*logger__th_start_routine(t_logger *NONNULL self)
{
	t_log	buf;
	t_ms	prev;

	prev = 0;
	while (logger__pop_queue(self, &buf))
	{
		if (buf.timestamp < prev)
			buf.timestamp = prev;
		prev = buf.timestamp;
		log_state(buf.coder_id, buf.timestamp, buf.state);
	}
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
		return (print_error("FAILED CREATING LOGGER THREAD", false));
	self->__thread_active = true;
	return (0);
}

int	logger__join_thread(t_logger *NONNULL self)
{
	if (!self->__thread_active)
		return (0);
	self->__thread_active = false;
	return (pthread_join(self->___thread, NULL));
}
