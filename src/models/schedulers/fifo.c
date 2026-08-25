/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 01:37:56 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 07:21:02 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fifo.h"

void	fifo__init(t_fifo *NONNULL self)
{
	self->super.__self = self;
	self->super.__put = (int (*)(void *, t_coder *))fifo__put;
	self->super.__pop = (void (*)(void *, t_coder *))fifo__pop;
	self->super.__delete = (void (*)(void *, t_coder *))fifo__delete;
	self->super.__first = (t_coder *(*)(void *))fifo__first;
	self->super.__reset = (void (*)(void *))fifo__reset;
	self->super.__destroy = (void (*)(void *))fifo__destroy;
	memset(self->__queue, 0, sizeof(self->__queue));
	self->__head = 0;
	self->__tail = 0;
	self->__size = 0;
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
