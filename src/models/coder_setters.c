/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_setters.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 07:52:15 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 21:46:19 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

void	coder__set_left_usb(t_coder *self, t_usb *usb)
{
	self->__left_usb = usb;
}

void	coder__set_right_usb(t_coder *self, t_usb *usb)
{
	self->__right_usb = usb;
}

void	coder__set_exit_thread(t_coder *NONNULL self, bool exit_thread)
{
	self->__exit_thread = exit_thread;
}
