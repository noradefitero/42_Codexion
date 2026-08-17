/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_conversion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:35:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 07:46:22 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "time.h"

struct timespec	timeval_to_timespec(const struct timeval *tv)
{
	struct timespec	ts;

	ts.tv_sec = tv->tv_sec;
	ts.tv_nsec = tv->tv_usec * 1000;
	return (ts);
}

struct timeval	timespec_to_timeval(const struct timespec *ts)
{
	struct timeval	tv;

	tv.tv_sec = ts->tv_sec;
	tv.tv_usec = ts->tv_nsec / 1000;
	return (tv);
}

t_ms	timeval_to_ms(const struct timeval *tv)
{
	t_ms	ms;

	ms = (t_ms)tv->tv_sec * 1000;
	ms += tv->tv_usec / 1000;
	return (ms);
}

struct timespec	ms_to_timespec(const t_ms ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}
