/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 23:54:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 09:13:30 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_H
# define LOGGER_H

# define LOG_POOL_MULTIPLIER 2
# define LOG_POOL_REALLOC_MULTIPLIER 1.5

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
}	t_log;

typedef struct s_logger
{
	pthread_t NULL_UNSPECIFIED	___thread;
	bool						__thread_active;
	bool						__exit_flag;
	pthread_mutex_t				__mutex;
	bool						__mutex_initialized;
	pthread_cond_t				__cond;
	bool						__cond_initialized;
	t_log *NULLABLE				__queue;
	bool						__queue_init;
	size_t						__pool_size;
	size_t						__head;
	size_t						__tail;
	size_t						__size;
}	t_logger;

int					logger__init(t_logger *NONNULL self, size_t n_coders);
t_logger *NULLABLE	logger__create(size_t n_coders);
void				logger__reset(t_logger *NONNULL self);
void				logger__destroy(t_logger *NONNULL logger);

/* QUEUE */
void				logger__add_to_queue(
						t_logger *NONNULL self,
						int coder_id,
						t_log_mess state
						);
int					logger__pop_queue(
						t_logger *NONNULL self,
						t_log *NONNULL buf
						);

/* THREAD LIFECYCLE */
int					logger__init_thread(t_logger *NONNULL self);
int					logger__join_thread(t_logger *NONNULL self);
int					logger__exit_thread(t_logger *NONNULL self);

#endif /* LOGGER_H */