/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 08:23:25 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 08:40:46 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft.h"

/*
 * This is the worst realloc you're going to see in your life. But since 42
 * does not allow any functions useful for reallocating memory, this only
 * allocates new memory and copies the old contents, regardless of whether
 * there was enough available memory to resize in place.
*/

void	*ft_realloc(void *ptr, size_t actual_size, size_t new_size)
{
	void	*new;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new = malloc(new_size);
	if (!new)
		return (NULL);
	if (ptr)
	{
		ft_memcpy(new, ptr, ft_minsize(actual_size, new_size));
		free(ptr);
	}
	return (new);
}
