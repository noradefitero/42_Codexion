/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:58:07 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 22:19:49 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "commands/commands.h"
#include "models/hub.h"
#include "utils/ft/ft.h"

#include <stdlib.h>

t_scheduler	str_to_scheduler(char const *s)
{
	char const	*s_sch[] = {"fifo", "edf", NULL};
	int			i;

	i = 0;
	while (s_sch[i])
	{
		if (!strcmp(s, s_sch[i]))
			return (i);
		i++;
	}
	return (UNKNOWN);
}

int	parse_arguments(t_config *config, int argc, char *argv[])
{
	int const		*iarg_arr[] = {&config->number_of_coders,
		&config->time_to_burnout, &config->time_to_compile,
		&config->time_to_debug, &config->time_to_refactor,
		&config->number_of_compiles_required, &config->dongle_cooldown, NULL};
	int				i;

	i = 0;
	if (argc != 8 && argc != 9)
		return (print_error("INVALID SINTAX. EXPECTING 7-8 ARGUMENTS", true));
	while (iarg_arr[i])
	{
		if (!ft_strisdigit(argv[i + 1]))
			return (print_error("INVALID NUMBER", true));
		*(int *)iarg_arr[i] = atoi(argv[i + 1]);
		i++;
	}
	if (argc == 9)
	{
		config->scheduler = str_to_scheduler(argv[8]);
		if (config->scheduler == UNKNOWN)
			return (print_error("INVALID SCHEDULER", true));
	}
	return (0);
}

int	main(int argc, char *argv[])
{
	t_config	config;
	t_hub		hub;

	if (argc < 2)
		return (help());
	if (argv[1][0] == '-')
		return (process_command(argv));
	if (parse_arguments(&config, argc, argv))
		return (1);
	if (hub__init(&hub, config.number_of_coders))
		return (print_error("ERROR INITIALIZING HUB", false));
	if (hub__coders_map(&hub, coder__init_thread))
	{
		hub__coders_map(&hub, coder__exit_thread);
		return (print_error("ERROR INITIALIZING THREADS", true));
	}
	hub__coders_map(&hub, coder__join_thread);
	hub__reset(&hub);
	return (0);
}
