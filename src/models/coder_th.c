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

/*
* Even coders grab their right dongle first, odd coders their left one.
* When every coder starts at the same instant, symmetric grabbing makes
* each of them hold one dongle while waiting for the other, so the last
* one in the chain waits N compile cycles and burns out in a feasible
* configuration. Asymmetric grabbing breaks that chain: neighbours that
* would wait on each other now contend directly, and losers hold nothing.
*/
static inline int	coder__th_compile(t_coder *NONNULL self)
{
	t_usb	*first;
	t_usb	*second;

	first = self->__left_usb;
	second = self->__right_usb;
	if (self->__id % 2 == 0)
	{
		first = self->__right_usb;
		second = self->__left_usb;
	}
	if (coder__th_own_usb(self, first))
		return (-1);
	if (coder__th_own_usb(self, second))
	{
		usb__release_safe(first, self);
		return (-1);
	}
	self->__last_compile = get_sim_time(false);
	logger__add_to_queue(self->__logger, self->__id, COMPILING);
	sleep_ms(self->__time_to_compile);
	usb__release_safe(self->__left_usb, self);
	usb__release_safe(self->__right_usb, self);
	self->__compiles++;
	self->__state = DEBUG;
	return (0);
}

static inline void	coder__th_debug(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, DEBUGGING);
	sleep_ms(self->__time_to_debug);
	self->__state = REFACTOR;
}

static inline void	coder__th_refactor(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, REFACTORING);
	sleep_ms(self->__time_to_refactor);
	self->__state = COMPILE;
}

/*
* N == 1: the single coder has no second dongle, so compiling is
* impossible. Take its only dongle once, then wait for the shutdown
* the monitor triggers once time_to_burnout passes.
*/
static void *NULLABLE	coder__th_n1_wait_burn(t_coder *NONNULL self)
{
	if (coder__th_own_usb(self, self->__left_usb))
		return (NULL);
	pthread_mutex_lock(usb__mutex(self->__left_usb));
	while (self->__left_usb->__active)
		pthread_cond_wait(usb__cond(self->__left_usb),
			usb__mutex(self->__left_usb));
	pthread_mutex_unlock(usb__mutex(self->__left_usb));
	usb__release_safe(self->__left_usb, self);
	return (NULL);
}

void *NULLABLE	coder__th_start_routine(t_coder *NONNULL self)
{
	if (self->__right_usb == NULL)
		return (coder__th_n1_wait_burn(self));
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
