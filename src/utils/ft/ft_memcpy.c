/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 17:09:27 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 03:51:16 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft.h"
#include <stdint.h>
#include <stdlib.h>

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const void	*_dest = dest;

	if (!dest && !src)
		return (NULL);
	while (n-- > 0)
		*((uint8_t *)dest++) = *((uint8_t *)src++);
	return ((void *)_dest);
}

// int	main(void)
// {
// 	char		*a;
// 	const char	b[12] = "Hola world!";

// 	a = malloc(1000 * sizeof(char));
// 	ft_memcpy(a, b, 1000);
// 	printf("String A(char): %s", a);
// 	free(a);
// 	return (0);
// }
