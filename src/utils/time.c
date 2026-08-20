/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 07:44:16 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 06:57:28 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "time.h"

t_ms	get_sim_time(bool reset)
{
	static t_ms	base;

	if (!base || reset)
		base = get_time();
	return (get_time() - base);
}
