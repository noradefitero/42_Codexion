/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:12:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/27 15:07:53 by dde-fite         ###   ########.fr       */
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

	pthread_mutex_lock(&mutex);
	if (burned)
	{
		pthread_mutex_unlock(&mutex);
		return ;
	}
	if (state == BURNED)
		burned = true;
	pthread_mutex_unlock(&mutex);
	if (state == TAKEN_DONGLE)
		printf("%lld %d %s\n", tm, n_coder, TAKEN_DONGLE_MESS);
	else if (state == COMPILING)
		printf("%lld %d %s\n", tm, n_coder, COMPILING_MESS);
	else if (state == DEBUGGING)
		printf("%lld %d %s\n", tm, n_coder, DEBUGGING_MESS);
	else if (state == REFACTORING)
		printf("%lld %d %s\n", tm, n_coder, REFACTORING_MESS);
	else if (state == BURNED)
		printf("%lld %d %s\n", tm, n_coder, BURNED_MESS);
}
