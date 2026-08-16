/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_getters.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:11:28 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 08:33:42 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

t_coder	**hub__coders(const t_hub *self)
{
	return (self->__coders);
}

t_usb	**hub__usbs(const t_hub *self)
{
	return (self->__usbs);
}

size_t	hub__n_coders(const t_hub *self)
{
	return (self->__n_coders);
}
