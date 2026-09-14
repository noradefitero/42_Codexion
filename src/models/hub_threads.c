/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_threads.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 12:00:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

int	hub__coders_map(t_hub *NONNULL self, int (*NONNULL f)(t_coder *NONNULL))
{
	const size_t	n_coders = self->__config.number_of_coders;
	size_t			i;

	i = 0;
	while (i < n_coders)
	{
		if (f(self->__coders[i]))
			return (1);
		i++;
	}
	return (0);
}

/*
* Start order matters: logger first so it can print from the very beginning,
* monitor second so it watches coders as soon as they start.
* On partial failure hub__run ends the simulation and joins what was started.
*/
int	hub__start_threads(t_hub *NONNULL self)
{
	if (logger__init_thread(&self->__logger))
		return (print_error("FAILED STARTING LOGGER THREAD", false));
	if (monitor__init_thread(&self->__monitor))
		return (print_error("FAILED STARTING MONITOR THREAD", false));
	if (hub__coders_map(self, coder__init_thread))
		return (print_error("FAILED STARTING A CODER THREAD", false));
	return (0);
}

/*
* Join in reverse dependency order: producers (coders) first, then the
* watcher (monitor), and the consumer (logger) last after requesting its
* stop so it drains every pending log before exiting.
*/
void	hub__join_threads(t_hub *NONNULL self)
{
	const size_t	n_coders = self->__config.number_of_coders;
	size_t			i;

	i = 0;
	while (i < n_coders)
	{
		coder__join_thread(self->__coders[i]);
		i++;
	}
	monitor__join_thread(&self->__monitor);
	logger__request_stop(&self->__logger);
	logger__join_thread(&self->__logger);
}

int	hub__run(t_hub *NONNULL self)
{
	int	status;

	pthread_mutex_lock(&self->__sim_mutex);
	self->__running = true;
	pthread_mutex_unlock(&self->__sim_mutex);
	status = hub__start_threads(self);
	if (status)
		hub__end(self);
	hub__wait_end(self);
	hub__join_threads(self);
	return (status);
}
