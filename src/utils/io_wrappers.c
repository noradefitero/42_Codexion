/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_wrappers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:12:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/12 03:29:46 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "io_wrappers.h"

int	print_error(char *msg, bool print_help)
{
	fprintf(stderr, "[1] ERROR: %s\n\n", msg);
	if (print_help)
		help();
	return (-1);
}
