/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 01:17:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 22:14:48 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

void *NULLABLE	coder__th_start_routine(t_coder *NONNULL self)
{
	printf("HI IM A CODER!\n");
	return (NULL);
}
