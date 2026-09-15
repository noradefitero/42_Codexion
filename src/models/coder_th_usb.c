/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th_usb.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 12:00:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "hub.h"

static inline int	coder__th_wait_dongle_cooldown(
	t_coder *NONNULL self,
	t_usb *NONNULL usb
)
{
	t_ms			deadline;
	struct timespec	ts_deadline;

	while (hub__is_running(self->__hub))
	{
		deadline = usb__cooldown_deadline(usb);
		if (deadline == 0 || deadline <= get_time())
			return (0);
		ts_deadline = ms_to_timespec(deadline);
		pthread_cond_timedwait(usb__cond(usb), usb__mutex(usb), &ts_deadline);
	}
	return (-1);
}

static int	coder__th_own_usb_failed(t_coder *NONNULL self, t_usb *NONNULL usb)
{
	usb__delete(usb, self);
	pthread_mutex_unlock(usb__mutex(usb));
	return (-1);
}

int	coder__th_own_usb(t_coder *NONNULL self, t_usb *NONNULL usb)
{
	pthread_mutex_lock(usb__mutex(usb));
	if (usb__acquire(usb, self))
	{
		pthread_mutex_unlock(usb__mutex(usb));
		return (-1);
	}
	while (usb__holder(usb) || usb__first(usb) != self)
	{
		if (!hub__is_running(self->__hub))
			return (coder__th_own_usb_failed(self, usb));
		pthread_cond_wait(usb__cond(usb), usb__mutex(usb));
	}
	if (coder__th_wait_dongle_cooldown(self, usb) == -1)
		return (coder__th_own_usb_failed(self, usb));
	usb__delete(usb, self);
	usb->__holder = self;
	pthread_mutex_unlock(usb__mutex(usb));
	logger__add_to_queue(self->__logger, self->__id, TAKEN_DONGLE);
	return (0);
}
