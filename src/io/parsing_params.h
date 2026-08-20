/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_params.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 01:23:48 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 23:04:39 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_PARAMS_H
# define PARSING_PARAMS_H

# include "codexion.h"
# include "print.h"
# include "../utils/ft/ft.h"

# include <stdlib.h>
# include <stdbool.h>
# include <string.h>

t_scheduler	str_to_scheduler(char const *s);
int			parse_arguments(t_config *config, int argc, char *argv[]);

#endif /* PARSING_PARAMS_H */