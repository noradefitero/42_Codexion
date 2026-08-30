/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/30 12:23:36 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "hub.h"

/*
* Once a coder reaches the head of the usb queue, the dongle may still be
* cooling down from its previous holder. Sleep until the cooldown deadline
* under the same mutex/cond so queue order is preserved and a shutdown
* broadcast (usb__wake from hub__end) can interrupt the wait promptly.
* Returns 0 on a normal cooldown elapse, or -1 if the simulation ended.
*/
static int	coder__th_wait_dongle_cooldown(
	t_coder *NONNULL self,
	t_usb *NONNULL usb
)
{
	struct timespec	deadline;
	t_ms			deadline_ms;
	t_ms			remaining;

	while (hub__is_running(self->__hub))
	{
		remaining = usb__cooldown_remaining(usb);
		if (remaining == 0)
			return (0);
		deadline_ms = get_time() + remaining;
		deadline = ms_to_timespec(deadline_ms);
		pthread_cond_timedwait(usb__cond(usb), usb__mutex(usb), &deadline);
	}
	return (-1);
}

/*
* Joins the usb queue and waits for the turn. Enqueueing, flag checks and the
* cond wait all happen under the same mutex hold so no wakeup is lost and no
* coder can be skipped while it is not waiting yet. After becoming head of the
* queue the coder also waits through any active dongle cooldown before logging
* TAKEN_DONGLE.
*/
static int	coder__th_own_usb(t_coder *NONNULL self, t_usb *NONNULL usb)
{
	pthread_mutex_lock(usb__mutex(usb));
	if (scheduler__put(usb__scheduler_unsafe(usb), self))
	{
		pthread_mutex_unlock(usb__mutex(usb));
		return (-1);
	}
	while (usb__first_unsafe(usb) != self)
	{
		if (!hub__is_running(self->__hub))
		{
			scheduler__delete(usb__scheduler_unsafe(usb), self);
			pthread_mutex_unlock(usb__mutex(usb));
			return (-1);
		}
		pthread_cond_wait(usb__cond(usb), usb__mutex(usb));
	}
	if (coder__th_wait_dongle_cooldown(self, usb) == -1)
	{
		scheduler__delete(usb__scheduler_unsafe(usb), self);
		pthread_mutex_unlock(usb__mutex(usb));
		return (-1);
	}
	pthread_mutex_unlock(usb__mutex(usb));
	logger__add_to_queue(self->__logger, self->__id, TAKEN_DONGLE);
	return (0);
}

static inline int	coder__th_compile(t_coder *NONNULL self)
{
	t_usb	*first;
	t_usb	*second;

	first = self->__left_usb;
	second = self->__right_usb;
	if (first > second)
	{
		first = self->__right_usb;
		second = self->__left_usb;
	}
	if (coder__th_own_usb(self, first))
		return (-1);
	if (coder__th_own_usb(self, second))
	{
		usb__delete(first, self);
		return (-1);
	}
	logger__add_to_queue(self->__logger, self->__id, COMPILING);
	usleep(ms_to_useconds(self->__time_to_compile));
	usb__release(self->__left_usb, self);
	usb__release(self->__right_usb, self);
	self->__last_compile = get_sim_time(false);
	self->__compiles++;
	self->__state = DEBUG;
	return (0);
}

static inline void	coder__th_debug(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, DEBUGGING);
	usleep(ms_to_useconds(self->__time_to_debug));
	self->__state = REFACTOR;
}

static inline void	coder__th_refactor(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, REFACTORING);
	usleep(ms_to_useconds(self->__time_to_refactor));
	self->__state = COMPILE;
}

void *NULLABLE	coder__th_start_routine(t_coder *NONNULL self)
{
	while (hub__is_running(self->__hub)
		&& self->__compiles < self->__number_of_compiles_required)
	{
		if (!self->__left_usb || !self->__right_usb)
		{
			usleep(MONITOR_TICK_US);
			continue ;
		}
		if (self->__state & COMPILE)
		{
			if (coder__th_compile(self))
				break ;
		}
		else if (self->__state & DEBUG)
			coder__th_debug(self);
		else if (self->__state & REFACTOR)
			coder__th_refactor(self);
	}
	return (NULL);
}
