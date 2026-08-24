/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:12:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 05:50:43 by dde-fite         ###   ########.fr       */
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

void	log_state(int n_coder, t_ms tm, t_log_mess state)
{
	static pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
	static bool				burned = false;
	const char				*template = "%d %d %s\n";

	pthread_mutex_lock(&mutex);
	if (burned)
		return ;
	if (state == TAKEN_DONGLE)
		printf(template, tm, n_coder, TAKEN_DONGLE_MESS);
	else if (state == COMPILING)
		printf(template, tm, n_coder, COMPILING_MESS);
	else if (state == DEBUGGING)
		printf(template, tm, n_coder, DEBUGGING_MESS);
	else if (state == REFACTORING)
		printf(template, tm, n_coder, REFACTORING_MESS);
	else if (state == BURNED)
	{
		fprintf(stderr, template, tm, n_coder, BURNED_MESS);
		burned = true;
	}
	pthread_mutex_unlock(&mutex);
}
