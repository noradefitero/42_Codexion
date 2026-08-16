/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_help.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 01:10:48 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/05 01:22:34 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands_help.h"

static void	print_commands_meta(void)
{
	const t_argument_meta	arg[] = {
	{VERSION_NAME, VERSION_DESC},
	{HELP_NAME, HELP_DESC}
	};
	size_t const			arg_size = sizeof(arg) / sizeof(arg[0]);
	size_t					i;

	printf("\nCommands:\n");
	i = 0;
	while (i < arg_size)
	{
		printf("    %s\n", arg[i].name);
		printf("        %s\n", arg[i].description);
		i++;
	}
}

static void	print_arguments_meta(void)
{
	const t_argument_meta	arg[] = {
	{NUMBER_OF_CODERS_NAME, NUMBER_OF_CODERS_DESC},
	{TIME_TO_BURNOUT_NAME, TIME_TO_BURNOUT_DESC},
	{TIME_TO_COMPILE_NAME, TIME_TO_COMPILE_DESC},
	{TIME_TO_DEBUG_NAME, TIME_TO_DEBUG_DESC},
	{TIME_TO_REFACTOR_NAME, TIME_TO_REFACTOR_DESC},
	{NUMBER_OF_COMPILES_REQUIRED_NAME, NUMBER_OF_COMPILES_REQUIRED_DESC},
	{DONGLE_COOLDOWN_NAME, DONGLE_COOLDOWN_DESC},
	{SCHEDULER_NAME, SCHEDULER_DESC}
	};
	size_t const			arg_size = sizeof(arg) / sizeof(arg[0]);
	size_t					i;

	printf("\nArguments:\n");
	i = 0;
	while (i < arg_size)
	{
		printf("    %s\n", arg[i].name);
		printf("        %s\n", arg[i].description);
		i++;
	}
}

int	help(void)
{
	printf ("%s Usage\n", PRGNAME);
	printf("\nUsage:\n");
	printf("    ./codexion -\\--<command> <arguments>\n");
	print_commands_meta();
	print_arguments_meta();
	return (0);
}
