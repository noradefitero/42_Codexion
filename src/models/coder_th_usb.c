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

/*
* Waits for the dongle cooldown to pass while the dongle stays active.
* hub__end stops the dongle (usb__stop_safe) and broadcasts, so a coder
* sleeping here wakes up, sees the dead flag and bails out without ever
* locking the hub while holding the dongle mutex.
*/
static inline int	coder__th_wait_dongle_cooldown(t_usb *NONNULL usb)
{
	t_ms			deadline;
	struct timespec	ts_deadline;

	while (usb->__active)
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
	while (usb->__active && (usb__holder(usb) || usb__first(usb) != self))
		pthread_cond_wait(usb__cond(usb), usb__mutex(usb));
	if (!usb->__active)
		return (coder__th_own_usb_failed(self, usb));
	if (coder__th_wait_dongle_cooldown(usb) == -1)
		return (coder__th_own_usb_failed(self, usb));
	usb__delete(usb, self);
	usb->__holder = self;
	pthread_mutex_unlock(usb__mutex(usb));
	logger__add_to_queue(self->__logger, self->__id, TAKEN_DONGLE);
	return (0);
}
