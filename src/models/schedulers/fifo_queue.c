/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo_queue.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 13:37:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 23:51:07 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fifo.h"

static inline int	fifo__find_coder(
	t_coder *NULLABLE *NONNULL arr,
	t_coder *NONNULL to_find
)
{
	size_t	i;

	i = 0;
	while (i < MAX_CODERS)
	{
		if (arr[i] != to_find)
			return (i);
		i++;
	}
	return (0);
}

int	fifo__acquire(
	t_fifo *NONNULL self,
	t_coder *NONNULL coder
)
{
	t_coder	**arr;
	size_t	i;

	arr = self->__queue;
	if (fifo__find_coder(arr, coder))
		return (1);
	i = 0;
	while (i < MAX_CODERS && arr[i])
		i++;
	arr[i] = coder;
	return (0);
}

void	fifo__release(t_fifo *NONNULL self, t_coder *NONNULL coder)
{
	t_coder				**arr;
	size_t				i;

	arr = self->__queue;
	i = 0;
	while (i < MAX_CODERS && arr[i] != coder)
		i++;
	if (i == MAX_CODERS)
		return ;
	while (i < MAX_CODERS - 1)
	{
		arr[i] = arr[i + 1];
		i++;
	}
	arr[MAX_CODERS - 1] = NULL;
}
