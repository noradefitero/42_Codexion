/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_queue.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 13:37:25 by dde-fite          #+#    #+#             */
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

int	edf__at(t_edf *NONNULL self, int index)
{
	return ((self->__head + index) % MAX_CODERS);
}

static int	edf__before(t_coder *NONNULL a, t_coder *NONNULL b)
{
	return (coder__deadline(a) < coder__deadline(b));
}

static void	edf__swap(t_edf *NONNULL self, int a, int b)
{
	t_coder	*tmp;

	tmp = self->__queue[edf__at(self, a)];
	self->__queue[edf__at(self, a)] = self->__queue[edf__at(self, b)];
	self->__queue[edf__at(self, b)] = tmp;
}

/* Bubble `index` up while its deadline is earlier than its parent's. */
void	edf__sift_up(t_edf *NONNULL self, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!edf__before(self->__queue[edf__at(self, index)],
				self->__queue[edf__at(self, parent)]))
			break ;
		edf__swap(self, index, parent);
		index = parent;
	}
}

/* Sink `index` down while a child has an earlier deadline. */
void	edf__sift_down(t_edf *NONNULL self, int index)
{
	int	left;
	int	right;
	int	smallest;

	while (true)
	{
		left = index * 2 + 1;
		right = left + 1;
		smallest = index;
		if (left < self->__size
			&& edf__before(self->__queue[edf__at(self, left)],
				self->__queue[edf__at(self, smallest)]))
			smallest = left;
		if (right < self->__size
			&& edf__before(self->__queue[edf__at(self, right)],
				self->__queue[edf__at(self, smallest)]))
			smallest = right;
		if (smallest == index)
			return ;
		edf__swap(self, index, smallest);
		index = smallest;
	}
}
