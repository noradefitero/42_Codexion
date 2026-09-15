/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 13:00:10 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

static int	usb__init_scheduler(t_usb *NONNULL self, t_config *NONNULL config)
{
	if (config->scheduler == FIFO)
		self->__scheduler = (t_scheduler *)fifo__create();
	else if (config->scheduler == EDF)
		self->__scheduler = (t_scheduler *)edf__create();
	if (!self->__scheduler)
	{
		usb__reset(self);
		return (1);
	}
	return (0);
}

int	usb__init(
	t_usb *NONNULL self,
	t_config *NONNULL config
)
{
	self->__dongle_cooldown = config->dongle_cooldown;
	self->__last_used = -1;
	self->__holder = NULL;
	self->__scheduler = NULL;
	self->__mutex_initialized = false;
	self->__cond_initialized = false;
	if (pthread_mutex_init(&self->__mutex, NULL))
	{
		usb__reset(self);
		return (print_error("FAILED INITIALIZING USB MUTEX", false));
	}
	self->__mutex_initialized = true;
	if (pthread_cond_init(&self->__cond, NULL))
	{
		usb__reset(self);
		return (print_error("FAILED INITIALIZING LOGGER COND", false));
	}
	self->__cond_initialized = true;
	if (usb__init_scheduler(self, config))
		return (1);
	self->__active = true;
	return (0);
}

t_usb *NULLABLE	usb__create(t_config *NONNULL config)
{
	t_usb	*result;

	result = (t_usb *)ft_calloc(1, sizeof(t_usb));
	if (!result)
	{
		print_error("FAILED ALLOCATING AN USB INSTANCE", false);
		return (NULL);
	}
	if (usb__init(result, config))
	{
		usb__destroy(result);
		return (NULL);
	}
	return (result);
}

void	usb__reset(t_usb *NONNULL self)
{
	self->__active = false;
	if (self->__mutex_initialized)
	{
		pthread_mutex_destroy(&self->__mutex);
		self->__mutex_initialized = false;
	}
	if (self->__cond_initialized)
	{
		pthread_cond_broadcast(&self->__cond);
		pthread_cond_destroy(&self->__cond);
		self->__cond_initialized = false;
	}
	if (self->__scheduler)
	{
		scheduler__destroy(self->__scheduler);
		self->__scheduler = NULL;
	}
}

void	usb__destroy(t_usb *NONNULL usb)
{
	if (usb)
	{
		usb__reset(usb);
		free(usb);
	}
}
