/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo_queue.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 13:37:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 07:17:51 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fifo.h"

int	fifo__put(
	t_fifo *NONNULL self,
	t_coder *NONNULL coder
)
{
	if (self->__size == MAX_CODERS)
		return (1);
	self->__queue[self->__tail] = coder;
	self->__tail = (self->__tail + 1) % MAX_CODERS;
	self->__size++;
	return (0);
}

void	fifo__pop(t_fifo *NONNULL self, t_coder *NONNULL coder)
{
	if (self->__size == 0 || self->__queue[self->__head] != coder)
		return ;
	self->__head = (self->__head + 1) % MAX_CODERS;
	self->__size--;
}

void	fifo__delete(t_fifo *NONNULL self, t_coder *NONNULL coder)
{
	int	i;
	int	n;
	int	size;

	size = self->__size;
	i = 0;
	n = 0;
	while (i < size)
	{
		if (self->__queue[(self->__head + i) % MAX_CODERS] != coder)
		{
			self->__queue[(self->__head + n) % MAX_CODERS]
				= self->__queue[(self->__head + i) % MAX_CODERS];
			n++;
		}
		i++;
	}
	i = n;
	while (i < size)
	{
		self->__queue[(self->__head + i) % MAX_CODERS] = NULL;
		i++;
	}
	self->__size = n;
	self->__tail = (self->__head + n) % MAX_CODERS;
}
