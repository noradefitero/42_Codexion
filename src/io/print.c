/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:12:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 01:28:37 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "print.h"

int	print_error(char *msg, bool print_help)
{
	fprintf(stderr, "[1] ERROR: %s\n\n", msg);
	if (print_help)
		help();
	return (-1);
}

int	log_state(int timestamp, int n_coder, t_log_mess state)
{
	static pthread_mutex_t	mutex;
	static bool				mutex_initialized;
	const char				*template = "%n %n %s";
	int						ret;

	ret = -1;
	if (!mutex_initialized)
		pthread_mutex_init(&mutex, NULL);
	pthread_mutex_lock(&mutex);
	if (state == TAKEN_DONLE)
		ret = printf(template, timestamp, n_coder, TAKEN_DONGLE_MESS);
	else if (state == COMPILING)
		ret = printf(template, timestamp, n_coder, COMPILING_MESS);
	else if (state == DEBUGGING)
		ret = printf(template, timestamp, n_coder, DEBUGGING_MESS);
	else if (state == REFACTORING)
		ret = printf(template, timestamp, n_coder, REFACTORING_MESS);
	else if (state == BURNED)
		ret = fprintf(stderr, template, timestamp, n_coder, BURNED_MESS);
	pthread_mutex_unlock(&mutex);
	return (ret);
}
