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

static int	edf__at(t_edf *NONNULL self, int index)
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
static void	edf__sift_up(t_edf *NONNULL self, int index)
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
static void	edf__sift_down(t_edf *NONNULL self, int index)
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
