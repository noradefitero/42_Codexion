/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 08:07:02 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

int	usb__init(
	t_usb *NONNULL self,
	t_config *NONNULL config,
	t_ms *NONNULL initial_time
)
{
	self->__dongle_cooldown = config->dongle_cooldown;
	self->__scheduler = config->scheduler;
	self->__initial_time = initial_time;
	if (pthread_mutex_init(&self->__mutex, NULL))
	{
		usb__reset(self);
		return (1);
	}
	self->__mutex_initialized = true;
	if (pthread_cond_init(&self->__cond, NULL))
	{
		usb__reset(self);
		return (1);
	}
	self->__cond_initialized = true;
	return (0);
}

t_usb *NULLABLE	usb__create(t_config *NONNULL config, int *NONNULL initial_time)
{
	t_usb	*result;

	result = (t_usb *)ft_calloc(1, sizeof(t_usb));
	if (!result)
		return (NULL);
	if (usb__init(result, config, initial_time))
	{
		usb__destroy(result);
		return (NULL);
	}
	return (result);
}

void	usb__reset(t_usb *NONNULL self)
{
	if (self->__mutex_initialized)
	{
		pthread_mutex_destroy(&self->__mutex);
		self->__mutex_initialized = false;
	}
	if (self->__cond_initialized)
	{
		pthread_cond_destroy(&self->__cond);
		self->__cond_initialized = false;
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

pthread_mutex_t	usb__mutex(const t_usb *NONNULL self)
{
	return (self->__mutex);
}
