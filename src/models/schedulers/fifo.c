/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 01:37:56 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 00:10:14 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fifo.h"

void	fifo__init(t_fifo *NONNULL self)
{
	self->super.__self = self;
	self->super.__acquire = (int (*)(void *, t_coder *))fifo__acquire;
	self->super.__release = (void (*)(void *, t_coder *))fifo__release;
	self->super.__first = (t_coder *(*)(void *))fifo__first;
	self->super.__reset = (void (*)(void *))fifo__reset;
	self->super.__destroy = (void (*)(void *))fifo__destroy;
	memset(self->__queue, 0, sizeof(self->__queue));
}

t_fifo *NULLABLE	fifo__create(void)
{
	t_fifo	*result;

	result = (t_fifo *)ft_calloc(1, sizeof(t_fifo));
	if (!result)
	{
		print_error("FAILED ALLOCATING A FIFO SCHEDULER INSTANCE", false);
		return (NULL);
	}
	fifo__init(result);
	return (result);
}

void	fifo__reset(t_fifo *NONNULL self)
{
}

void	fifo__destroy(t_fifo *NONNULL fifo)
{
	if (fifo)
	{
		fifo__reset(fifo);
		free(fifo);
	}
}
