/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_wrappers.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:13:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/11 19:56:41 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_WRAPPERS_H
# define IO_WRAPPERS_H

# include "../commands/commands_help.h"

# include <stdbool.h>
# include <stdio.h>

int	print_error(char *msg, bool print_help);

#endif /* IO_WRAPPERS_H */