/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/10 23:37:52 by dde-fite         ###   ########.fr       */
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

static int	coder__th_own_usb(t_coder *NONNULL self, t_usb *NONNULL usb)
{
	pthread_mutex_lock(usb__mutex(usb));
	if (usb__acquire(usb, self))
	{
		pthread_mutex_unlock(usb__mutex(usb));
		return (-1);
	}
	while (usb__first(usb) != self)
	{
		if (!hub__is_running(self->__hub))
		{
			usb__delete(usb, self);
			pthread_mutex_unlock(usb__mutex(usb));
			return (-1);
		}
		pthread_cond_wait(usb__cond(usb), usb__mutex(usb));
	}
	if (coder__th_wait_dongle_cooldown(self, usb) == -1)
	{
		usb__delete(usb, self);
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
		usb__delete_safe(first, self);
		return (-1);
	}
	logger__add_to_queue(self->__logger, self->__id, COMPILING);
	usleep(ms_to_useconds(self->__time_to_compile));
	usb__release_safe(self->__left_usb, self);
	usb__release_safe(self->__right_usb, self);
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
