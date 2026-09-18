/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_conversions.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 06:58:39 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/16 16:41:40 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_CONVERSIONS_H
# define TIME_CONVERSIONS_H

# include <stdbool.h>
# include <unistd.h>
# include <sys/time.h>

typedef long long	t_ms;

/*
* Returns a new timespec with values of tv.
* It converts the nanoseconds of timeval to microseconds.
*/
static inline struct timespec	timeval_to_timespec(const struct timeval *tv)
{
	struct timespec	ts;

	ts.tv_sec = tv->tv_sec;
	ts.tv_nsec = tv->tv_usec * 1000;
	return (ts);
}

/*
* Returns a new timeval with values of ts.
* It converts the microseconds of timespec to nanoseconds.
*/
static inline struct timeval	timespec_to_timeval(const struct timespec *ts)
{
	struct timeval	tv;

	tv.tv_sec = ts->tv_sec;
	tv.tv_usec = ts->tv_nsec / 1000;
	return (tv);
}

/*
* Returns a t_ms with values of tv.
* It converts seconds and to microseconds to miliseconds.
*/
static inline t_ms	timeval_to_ms(const struct timeval *tv)
{
	t_ms	ms;

	ms = (t_ms)tv->tv_sec * 1000;
	ms += tv->tv_usec / 1000;
	return (ms);
}

/*
* Returns a timespec with values of ms.
* It converts miliseconds to seconds and nanoseconds.
*/
static inline struct timespec	ms_to_timespec(const t_ms ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}

#endif /* TIME_CONVERSIONS_H */
