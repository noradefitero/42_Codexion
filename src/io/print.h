/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:13:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 05:50:31 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_H
# define PRINT_H

# include "../commands/commands_help.h"
# include "../utils/time.h"

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>

typedef enum e_log_mess
{
	TAKEN_DONGLE,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNED
}	t_log_mess;

# define TAKEN_DONGLE_MESS "has taken a dongle"
# define COMPILING_MESS "is compiling"
# define DEBUGGING_MESS "is debugging"
# define REFACTORING_MESS "is refactoring"
# define BURNED_MESS "burned out"

int		print_error(char *msg, bool print_help);
void	log_state(int n_coder, t_ms tm, t_log_mess state);

#endif /* PRINT_H */
