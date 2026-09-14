/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_ops.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 12:00:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * EDF is a min-heap living on the same rotating window as FIFO
 * (head, tail, size). All positions are logical heap indexes; the
 * physical slot is (head + index) % MAX_CODERS, so the heap rotates
 * freely over the circular buffer and the freed slots are reused.
 * put / pop / delete are O(log n) thanks to sift_up / sift_down.
 */

#include "edf.h"
#include "../coder.h"

int	edf__put(
	t_edf *NONNULL self,
	t_coder *NONNULL coder
)
{
	if (self->__size == MAX_CODERS)
		return (1);
	self->__queue[edf__at(self, self->__size)] = coder;
	self->__size++;
	self->__tail = edf__at(self, self->__size);
	edf__sift_up(self, self->__size - 1);
	return (0);
}

void	edf__pop(t_edf *NONNULL self, t_coder *NONNULL coder)
{
	int	last;

	if (self->__size == 0 || self->__queue[self->__head] != coder)
		return ;
	self->__size--;
	last = edf__at(self, self->__size);
	self->__queue[self->__head] = self->__queue[last];
	self->__queue[last] = NULL;
	self->__tail = last;
	edf__sift_down(self, 0);
}

void	edf__delete(t_edf *NONNULL self, t_coder *NONNULL coder)
{
	int	i;
	int	last;

	i = 0;
	while (i < self->__size
		&& self->__queue[edf__at(self, i)] != coder)
		i++;
	if (i == self->__size)
		return ;
	self->__size--;
	last = edf__at(self, self->__size);
	if (edf__at(self, i) != last)
	{
		self->__queue[edf__at(self, i)] = self->__queue[last];
		edf__sift_up(self, i);
		edf__sift_down(self, i);
	}
	self->__queue[last] = NULL;
	self->__tail = last;
}
