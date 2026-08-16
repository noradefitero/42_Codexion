/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_conversion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:35:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/07/30 22:22:48 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "time_conversion.h"

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
