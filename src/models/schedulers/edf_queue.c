/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf_queue.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:03:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 06:19:54 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "edf.h"

static inline t_edf_node *NULLABLE	edf__new_node(t_coder *NONNULL content)
{
	t_edf_node	*node;

	node = ft_calloc(1, sizeof(t_edf_node));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

void	edf__update(t_edf *NONNULL self)
{
	(void)self;
}

int	edf__put(
	t_edf *NONNULL self,
	t_coder *NONNULL coder
)
{
	t_edf_node *NULLABLE	new;

	if (!self->__active)
		return (1);
	new = edf__new_node(coder);
	if (!new)
		return (1);
	if (self->__queue_tail)
		self->__queue_tail->next = new;
	else
		self->__queue = new;
	self->__queue_tail = new;
	edf__update(self);
	return (0);
}

void	edf__pop(t_edf *NONNULL self, t_coder *NONNULL coder)
{
	t_edf_node	*current;
	t_edf_node	*previous;

	if (!self->__active)
		return ;
	previous = NULL;
	current = self->__queue;
	while (current)
	{
		if (current->content == coder)
		{
			if (previous)
				previous->next = current->next;
			else
				self->__queue = current->next;
			if (!current->next)
				self->__queue_tail = previous;
			free(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
	edf__update(self);
}

void	edf__clear_queue(t_edf *NONNULL self)
{
	t_edf_node	*current;
	t_edf_node	*next;

	self->__active = false;
	current = self->__queue;
	self->__queue = NULL;
	self->__queue_tail = NULL;
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
	self->__queue = NULL;
}
