/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_conversion.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 21:56:57 by dde-fite          #+#    #+#             */
/*   Updated: 2026/07/30 22:14:26 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_CONVERSION_H
# define TIME_CONVERSION_H

# include <sys/time.h>
# include <time.h>

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

#endif /* TIME_CONVERSION_H */