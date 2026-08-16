/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 04:42:32 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/14 07:12:14 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# define PRGNAME "codexion"
# define PRGVERSION "0.1"

typedef enum e_scheduler
{
	FIFO,
	EDF,
	UNKNOWN
}	t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

// typedef struct s_data
// {
// 	int					nbr;
// 	struct timespec		timeout;
// 	pthread_mutex_t		mutex_incrementer;
// 	pthread_mutex_t		mutex_finish;
// 	pthread_cond_t		cond;
// }	t_data;

#endif /* CODEXION_H */