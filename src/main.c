/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:58:07 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 07:39:32 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "commands/commands.h"
#include "models/hub.h"
#include "io/print.h"
#include "io/parsing_params.h"

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
	if (config.number_of_coders == 0 || config.number_of_compiles_required == 0)
		return (0);
	if (hub__init(&hub, &config))
		return (-1);
	if (hub__coders_map(&hub, coder__init_thread))
	{
		hub__reset(&hub);
		return (-1);
	}
	monitor__init_thread(hub__monitor(&hub));
	logger__init_thread(hub__logger(&hub));
	hub__coders_map(&hub, coder__join_thread);
	monitor__exit_thread(hub__monitor(&hub));
	logger__exit_thread(hub__logger(&hub));
	hub__reset(&hub);
	return (0);
}
