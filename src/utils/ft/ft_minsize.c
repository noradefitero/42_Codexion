/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_minsize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 20:52:58 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 08:38:44 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft.h"

size_t	ft_minsize(size_t n1, size_t n2)
{
	if (n1 < n2)
		return (n1);
	return (n2);
}

// #include <stdio.h>

// int	main(void)
// {
// 	const int	a = 146;
// 	const int	b = 323;

// 	printf("Nº1: %d, Nº2: %d. Min: %d", a, b, ft_minnbr(a, b));
// }
