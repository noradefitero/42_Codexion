/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 23:54:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 07:07:31 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_H
# define LOGGER_H

# include "annotations.h"
# include "../io/print.h"
# include "../utils/ft/ft.h"

# include <pthread.h>
# include <stdlib.h>

typedef struct s_log
{
	int						coder_id;
	t_ms					timestamp;
	t_log_mess				state;
	struct s_log *NULLABLE	next;
}	t_log;

typedef struct s_logger
{
	pthread_t NULL_UNSPECIFIED	___thread;
	bool						__thread_active;
	bool						__exit_flag;
	pthread_mutex_t				__mutex;
	bool						__mutex_initialized;
	// pthread_cond_t
	t_log *NULLABLE				__queue;
	t_log *NULLABLE				__queue_tail;
	bool						__queue_active;
}	t_logger;

int					logger__init(t_logger *NONNULL self);
t_logger *NULLABLE	logger__create(void);
void				logger__reset(t_logger *NONNULL self);
void				logger__destroy(t_logger *NONNULL logger);

/* QUEUE */
int					logger__add_to_queue(
						t_logger *NONNULL self,
						int coder_id,
						t_log_mess state
						);
t_log *NULLABLE		logger__pop_queue(t_logger *NONNULL self);
void				logger__clear_queue(t_logger *NONNULL self);

/* THREAD LIFECYCLE */
int					logger__init_thread(t_logger *NONNULL self);
int					logger__join_thread(t_logger *NONNULL self);
int					logger__exit_thread(t_logger *NONNULL self);

#endif /* LOGGER_H */