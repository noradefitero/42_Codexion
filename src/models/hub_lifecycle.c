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

static int	hub__create_coders_loop(
	t_hub *NONNULL self,
	t_coder *NONNULL *NONNULL buf
)
{
	const t_usb		**usbs = (const t_usb **)self->__usbs;
	const size_t	n_coders = self->__config.number_of_coders;
	size_t			i;

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
	return (0);
}

int	hub__create_coders(t_hub *NONNULL self)
{
	const size_t	n_coders = self->__config.number_of_coders;
	t_coder			**buf;

	buf = (t_coder **)ft_calloc(n_coders, sizeof(void *));
	if (!buf)
		return (print_error("FAILED ALLOCATING CODERS LIST", false));
	if (hub__create_coders_loop(self, buf))
		return (1);
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
