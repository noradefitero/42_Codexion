/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_events.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:39:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/18 01:38:59 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

void	hub__on_burn(t_hub *NONNULL self)
{
	hub__coders_map(self, coder__exit_thread);
}
