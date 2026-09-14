/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_setters.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 07:52:15 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/18 01:53:06 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

void	coder__set_left_usb(t_coder *self, const t_usb *usb)
{
	self->__left_usb = (t_usb *)usb;
}

void	coder__set_right_usb(t_coder *self, const t_usb *usb)
{
	self->__right_usb = (t_usb *)usb;
}

void	coder__set_hub(t_coder *NONNULL self, t_hub *NULLABLE hub)
{
	self->__hub = hub;
}
