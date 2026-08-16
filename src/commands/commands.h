/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 22:32:34 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/09 20:17:03 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_H
# define COMMANDS_H

# include "../utils/io_wrappers.h"
# include "codexion.h"

# include <stdio.h>
# include <string.h>

/* COMMANDS */
# include "commands_help.h"

int	version(void);

int	process_command(char *argv[]);

#endif /* COMMANDS_H */
