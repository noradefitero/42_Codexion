/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 07:58:35 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

static inline int	coder__th_wait_usb(
	t_coder *NONNULL self,
	t_usb *NONNULL usb
)
{
	pthread_mutex_lock(usb__mutex(usb));
	while (usb__first(usb) != self)
	{
		if (self->__exit_thread || !usb__active(usb))
		{
			pthread_mutex_unlock(usb__mutex(usb));
			return (-1);
		}
		pthread_cond_wait(usb__cond(usb), usb__mutex(usb));
	}
	pthread_mutex_unlock(usb__mutex(usb));
	logger__add_to_queue(self->__logger, self->__id, TAKEN_DONGLE);
	return (0);
}

static inline int	coder__th_compile(
	t_coder *NONNULL self
)
{
	t_usb	*first;
	t_usb	*second;
	bool	single;

	first = self->__left_usb;
	second = self->__right_usb;
	single = (first == second);
	if (first > second)
	{
		first = self->__right_usb;
		second = self->__left_usb;
	}
	pthread_mutex_lock(usb__mutex(first));
	if (!single)
		pthread_mutex_lock(usb__mutex(second));
	if (usb__acquire(first, self))
	{
		if (!single)
			pthread_mutex_unlock(usb__mutex(second));
		pthread_mutex_unlock(usb__mutex(first));
		return (-1);
	}
	if (!single && usb__acquire(second, self))
	{
		pthread_mutex_unlock(usb__mutex(second));
		pthread_mutex_unlock(usb__mutex(first));
		usb__release(first, self);
		return (-1);
	}
	if (!single)
		pthread_mutex_unlock(usb__mutex(second));
	pthread_mutex_unlock(usb__mutex(first));
	if (coder__th_wait_usb(self, self->__left_usb)
		|| coder__th_wait_usb(self, self->__right_usb))
	{
		usb__release(self->__left_usb, self);
		usb__release(self->__right_usb, self);
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
	self->__last_compile = get_sim_time(false);
	while (!self->__left_usb || !self->__right_usb)
		usleep(ms_to_useconds(self->__time_to_burnout));
	while (!self->__exit_thread
		&& self->__compiles < self->__number_of_compiles_required)
	{
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
	self->__thread_active = false;
	return (NULL);
}
