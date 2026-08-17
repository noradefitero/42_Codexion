/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 07:44:16 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 07:48:52 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "time.h"

t_ms	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (timeval_to_ms(&tv));
}
