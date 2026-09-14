/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 23:54:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 11:48:46 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MONITOR_H
# define MONITOR_H

# include "annotations.h"
# include "coder.h"
# include "logger.h"
# include "../io/print.h"

# include <pthread.h>
# include <stdlib.h>

# define MONITOR_TICK_US 1000

typedef struct s_hub	t_hub;

typedef struct s_monitor
{
	pthread_t NULL_UNSPECIFIED		___thread;
	bool							__thread_active;
	t_coder *NONNULL *NONNULL		__coders;
	size_t							__n_coders;
	int								__time_to_burnout;
	t_logger *NULLABLE				__logger;
	t_hub *NULLABLE					__hub; /* t_hub * */
}	t_monitor;

void				monitor__init(
						t_monitor *NONNULL self,
						t_coder *NONNULL *NONNULL coders,
						t_config *NONNULL config,
						t_logger *NULLABLE logger
						);
t_monitor *NULLABLE	monitor__create(
						t_coder *NONNULL *NONNULL coders,
						t_config *NONNULL config,
						t_logger *NULLABLE logger,
						t_hub *NULLABLE hub
						);
void				monitor__reset(t_monitor *NONNULL self);
void				monitor__destroy(t_monitor *NULLABLE monitor);
void				monitor__set_hub(
						t_monitor *NONNULL self,
						t_hub *NULLABLE hub
						);

/* THREAD LIFECYCLE */
int					monitor__init_thread(t_monitor *NONNULL self);
int					monitor__join_thread(t_monitor *NONNULL self);

#endif /* MONITOR_H */
