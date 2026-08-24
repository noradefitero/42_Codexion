/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb_api.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 09:22:49 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 08:02:04 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "usb.h"

int	usb__acquire(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	int	ret;

	ret = scheduler__acquire(self->__scheduler, coder);
	if (ret)
		print_error("SCHEDULER FAILED TO ADD A CODER TO THE QUEUE", false);
	return (ret);
}

void	usb__release(
	t_usb *NONNULL self,
	t_coder *NONNULL coder
)
{
	pthread_mutex_lock(&self->__mutex);
	scheduler__release(self->__scheduler, coder);
	pthread_cond_broadcast(&self->__cond);
	pthread_mutex_unlock(&self->__mutex);
}
