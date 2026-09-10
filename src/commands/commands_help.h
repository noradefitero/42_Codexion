/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_help.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 01:11:08 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/30 19:32:44 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HELP_H
# define COMMANDS_HELP_H

# include "codexion.h"

# include <stdio.h>

typedef struct s_command_meta
{
	const char	*name;
	const char	*description;
}	t_command_meta;

/* META OF COMMANDS */
# define HELP_NAME \
	"help"
# define HELP_DESC \
	"Prints all available options for using this program."

# define VERSION_NAME \
	"version"
# define VERSION_DESC \
	"Prints program's version."

typedef struct s_argument_meta
{
	const char	*name;
	const char	*description;
}	t_argument_meta;

/* META OF ARGUMENTS */
# define NUMBER_OF_CODERS_NAME \
	"number_of_coders"
# define NUMBER_OF_CODERS_DESC \
	"Number of coders (and dongles)."

# define TIME_TO_BURNOUT_NAME \
	"time_to_burnout"
# define TIME_TO_BURNOUT_DESC \
	"Time in milliseconds a coder can wait before burning out."

# define TIME_TO_COMPILE_NAME \
	"time_to_compile"
# define TIME_TO_COMPILE_DESC \
	"Time in milliseconds a coder spends compiling."

# define TIME_TO_DEBUG_NAME \
	"time_to_debug"
# define TIME_TO_DEBUG_DESC \
	"Time in milliseconds a coder spends debugging."

# define TIME_TO_REFACTOR_NAME \
	"time_to_refactor"
# define TIME_TO_REFACTOR_DESC \
	"Time in milliseconds a coder spends refactoring."

# define NUMBER_OF_COMPILES_REQUIRED_NAME \
	"numbers_of_compiles_required"
# define NUMBER_OF_COMPILES_REQUIRED_DESC \
	"Simulation stops once every coder has compiled this many times."

# define DONGLE_COOLDOWN_NAME \
	"dongle_cooldown"
# define DONGLE_COOLDOWN_DESC \
	"Time in milliseconds a dongle remains unavailable after release."

# define SCHEDULER_NAME \
	"scheduler"
# define SCHEDULER_DESC \
	"Scheduler: fifo or edf"

int	help(void);

#endif /* COMMANDS_HELP_H */