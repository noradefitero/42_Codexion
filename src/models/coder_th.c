/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/28 21:02:50 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "hub.h"

/*
* Joins the usb queue and waits for the turn. Enqueueing, flag checks and the
* cond wait all happen under the same mutex hold so no wakeup is lost and no
* coder can be skipped while it is not waiting yet.
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
	pthread_mutex_unlock(usb__mutex(usb));
	logger__add_to_queue(self->__logger, self->__id, TAKEN_DONGLE);
	return (0);
}

static int	coder__th_compile(t_coder *NONNULL self)
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

static void	coder__th_debug(t_coder *NONNULL self)
{
	logger__add_to_queue(self->__logger, self->__id, DEBUGGING);
	usleep(ms_to_useconds(self->__time_to_debug));
	self->__state = REFACTOR;
}

static void	coder__th_refactor(t_coder *NONNULL self)
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
		if (self->__state == COMPILE)
		{
			if (coder__th_compile(self))
				break ;
		}
		else if (self->__state == DEBUG)
			coder__th_debug(self);
		else if (self->__state == REFACTOR)
			coder__th_refactor(self);
	}
	return (NULL);
}
