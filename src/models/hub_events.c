/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_events.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:39:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 07:40:40 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

void	hub__on_burn(t_hub *NONNULL self)
{
	const size_t	n_usbs = self->__config.number_of_coders;
	size_t			i;

	hub__coders_map(self, coder__exit_thread);
	i = 0;
	while (i < n_usbs)
		usb__wake(self->__usbs[i++]);
	logger__exit_thread(&self->__logger);
	monitor__exit_thread(&self->__monitor);
}
