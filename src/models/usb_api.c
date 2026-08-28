/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb_api.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 09:22:49 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:58:46 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

/*
* Scheduler peek without locking: the caller must already hold the usb mutex,
* which is how the coder threads check their turn and wait atomically.
*/
t_coder *NULLABLE	usb__first_unsafe(t_usb *NONNULL self)
{
	return (scheduler__first(self->__scheduler));
}

/* Broadcasts the cond under the mutex so no waiting coder misses it. */
void	usb__wake(t_usb *NONNULL self)
{
	pthread_mutex_lock(&self->__mutex);
	if (self->__cond_initialized)
		pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

int	usb__acquire(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	int	ret;

	pthread_mutex_lock(&self->__mutex);
	ret = scheduler__put(self->__scheduler, coder);
	if (ret)
		print_error("SCHEDULER FAILED TO ADD A CODER TO THE QUEUE", false);
	pthread_mutex_unlock(&self->__mutex);
	return (ret);
}

void	usb__release(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	pthread_mutex_lock(&self->__mutex);
	scheduler__pop(self->__scheduler, coder);
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

void	usb__delete(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	pthread_mutex_lock(&self->__mutex);
	scheduler__delete(self->__scheduler, coder);
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

// int usb__take
