/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 08:32:54 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

int	usb__init(t_usb *NONNULL self)
{
	if (pthread_mutex_init(&self->__mutex, NULL))
		return (1);
	self->__mutex_initialized = true;
	return (0);
}

t_usb *NULLABLE	usb__create(void)
{
	t_usb	*result;

	result = (t_usb *)ft_calloc(1, sizeof(t_usb));
	if (!result)
		return (NULL);
	if (usb__init(result))
	{
		usb__destroy(result);
		return (NULL);
	}
	return (result);
}

void	usb__reset(t_usb *NONNULL self)
{
	if (self->__mutex_initialized)
		pthread_mutex_destroy(&self->__mutex);
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
