/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb_api.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 09:22:49 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/10 23:19:25 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

int	usb__acquire(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	int	ret;

	ret = scheduler__put(self->__scheduler, coder);
	if (ret)
		print_error("SCHEDULER FAILED TO ADD A CODER TO THE QUEUE", false);
	return (ret);
}

void	usb__delete(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	scheduler__delete(self->__scheduler, coder);
	pthread_cond_broadcast(&self->__cond);
}

void	usb__release_safe(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	pthread_mutex_lock(&self->__mutex);
	if (self->__holder == coder)
		self->__holder = NULL;
	self->__last_used = get_time();
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

void	usb__delete_safe(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	pthread_mutex_lock(&self->__mutex);
	scheduler__delete(self->__scheduler, coder);
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}

/* Broadcasts the cond under the mutex so no waiting coder misses it. */
void	usb__wake_safe(t_usb *NONNULL self)
{
	pthread_mutex_lock(&self->__mutex);
	if (self->__cond_initialized)
		pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}
