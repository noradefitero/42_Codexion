/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo_queue.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 13:37:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 06:30:39 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fifo.h"

static inline t_fifo_node *NULLABLE	fifo__new_node(t_coder *NONNULL content)
{
	t_fifo_node	*node;

	node = ft_calloc(1, sizeof(t_fifo_node));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

int	fifo__acquire(
	t_fifo *NONNULL self,
	t_coder *NONNULL coder
)
{
	t_fifo_node *NULLABLE	new;

	if (!self->__active)
		return (1);
	new = fifo__new_node(coder);
	if (!new)
		return (1);
	if (self->__queue_tail)
		self->__queue_tail->next = new;
	else
		self->__queue = new;
	self->__queue_tail = new;
	return (0);
}

void	fifo__release(t_fifo *NONNULL self, t_coder *NONNULL coder)
{
	t_fifo_node	*current;
	t_fifo_node	*previous;

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
}

void	fifo__clear_queue(t_fifo *NONNULL self)
{
	t_fifo_node	*current;
	t_fifo_node	*next;

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
