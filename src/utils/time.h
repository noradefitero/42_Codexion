/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:56:57 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 07:01:20 by dde-fite         ###   ########.fr       */
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

#endif /* TIME_H */