/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub_lifecycle.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 08:39:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 12:08:21 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hub.h"

int	hub__create_usbs(t_hub *NONNULL self)
{
	const size_t	n_usbs = self->__config.number_of_coders;
	t_usb			**buf;
	size_t			i;

	buf = (t_usb **)ft_calloc(n_usbs, sizeof(void *));
	if (!buf)
		return (print_error("FAILED ALLOCATING USBS LIST", false));
	i = 0;
	while (i < n_usbs)
	{
		buf[i] = usb__create(&self->__config);
		if (!buf[i])
		{
			hub__destroy_usbs(buf, i);
			free(buf);
			return (1);
		}
		i++;
	}
	self->__usbs = buf;
	return (0);
}

int	hub__create_coders(t_hub *NONNULL self)
{
	const t_usb		**usbs = (const t_usb **)self->__usbs;
	const size_t	n_coders = self->__config.number_of_coders;
	t_coder			**buf;
	size_t			i;

	buf = (t_coder **)ft_calloc(n_coders, sizeof(void *));
	if (!buf)
		return (print_error("FAILED ALLOCATING CODERS LIST", false));
	i = 0;
	while (i < n_coders)
	{
		buf[i] = coder__create(
				i + 1, &self->__config, &self->__logger, self);
		if (!buf[i])
		{
			hub__destroy_coders(buf, i);
			free(buf);
			return (1);
		}
		coder__set_left_usb(buf[i], usbs[i]);
		if (self->__config.number_of_coders > 1)
			coder__set_right_usb(buf[i], usbs[(i + 1) % n_coders]);
		i++;
	}
	self->__coders = buf;
	return (0);
}

void	hub__destroy_usbs(t_usb *NONNULL *NONNULL usbs, size_t n_usbs)
{
	size_t	i;

	i = 0;
	while (i < n_usbs)
		usb__destroy(usbs[i++]);
}

void	hub__destroy_coders(t_coder *NULLABLE *NONNULL coders, size_t n_coders)
{
	size_t	i;

	i = 0;
	while (i < n_coders)
		coder__destroy(coders[i++]);
}

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
