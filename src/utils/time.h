/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:56:57 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 07:49:12 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_H
# define TIME_H

# include "codexion.h"

# include <sys/time.h>
# include <time.h>

t_ms			get_time(void);

/* TIME CONVERSIONS*/

/*
 * Returns a new timespec with values of tv.
 * It converts the nanoseconds of timeval to microseconds.
*/
struct timespec	timeval_to_timespec(const struct timeval *tv);

/*
 * Returns a new timeval with values of ts.
 * It converts the microseconds of timespec to nanoseconds.
*/
struct timeval	timespec_to_timeval(const struct timespec *ts);

/*
 * Returns a t_ms with values of tv.
 * It converts seconds and to microseconds to miliseconds.
*/
t_ms			timeval_to_ms(const struct timeval *tv);

/*
 * Returns a timespec with values of ms.
 * It converts miliseconds to seconds and nanoseconds.
*/
struct timespec	ms_to_timespec(const t_ms ms);

#endif /* TIME_H */