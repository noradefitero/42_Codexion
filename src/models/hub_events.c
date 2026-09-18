/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_events.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:39:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/10 23:16:24 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

bool	hub__is_running(t_hub *NONNULL self)
{
	bool	running;

	pthread_mutex_lock(&self->__sim_mutex);
	running = self->__running;
	pthread_mutex_unlock(&self->__sim_mutex);
	return (running);
}

/*
* Single shutdown entry point, idempotent. Flags the end of the simulation,
* wakes main (sim cond), every coder blocked on an usb queue (under its own
* mutex so no wakeup is lost) and the logger.
*/
void	hub__end(t_hub *NONNULL self)
{
	const size_t	n_usbs = self->__config.number_of_coders;
	size_t			i;

	pthread_mutex_lock(&self->__sim_mutex);
	if (!self->__running)
	{
		pthread_mutex_unlock(&self->__sim_mutex);
		return ;
	}
	self->__running = false;
	pthread_mutex_unlock(&self->__sim_mutex);
	pthread_cond_broadcast(&self->__sim_cond);
	i = 0;
	while (i < n_usbs)
	{
		usb__stop_safe(self->__usbs[i]);
		i++;
	}
	logger__wake(&self->__logger);
}

void	hub__wait_end(t_hub *NONNULL self)
{
	pthread_mutex_lock(&self->__sim_mutex);
	while (self->__running)
		pthread_cond_wait(&self->__sim_cond, &self->__sim_mutex);
	pthread_mutex_unlock(&self->__sim_mutex);
}
