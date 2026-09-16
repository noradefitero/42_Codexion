/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:56:57 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/16 16:41:49 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_H
# define TIME_H

# include "time_conversions.h"
# include "codexion.h"

# include <stdbool.h>
# include <sys/time.h>

typedef long long	t_ms;

static inline t_ms	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (timeval_to_ms(&tv));
}
t_ms					get_sim_time(bool reset);

/*
* Sleeps ms miliseconds. usleep rejects arguments of one second or more,
* so longer sleeps are split into sub-second chunks.
*/
static inline void	sleep_ms(const t_ms ms)
{
	t_ms	left;

	left = ms;
	while (left >= 1000)
	{
		usleep(999000);
		left -= 999;
	}
	usleep(left * 1000);
}

#endif /* TIME_H */