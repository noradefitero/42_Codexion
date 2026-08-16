/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_getters.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 07:48:12 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 21:45:34 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

t_usb	*coder__left_usb(const t_coder *self)
{
	return (self->__left_usb);
}

t_usb	*coder__right_usb(const t_coder *self)
{
	return (self->__right_usb);
}

t_coder_state	coder__state(const t_coder *self)
{
	return (self->__state);
}
