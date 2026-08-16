/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 22:28:10 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/11 19:59:14 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.h"

int	version(void)
{
	printf("%s v%s\n", PRGNAME, PRGVERSION);
	return (0);
}

int	process_command(char *argv[])
{
	if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version"))
		return (version());
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
		return (help());
	return (print_error("UNKNOWN COMMAND", true));
}
