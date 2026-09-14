/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_queue.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 13:37:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 21:01:06 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
