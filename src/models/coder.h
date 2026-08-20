/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 03:19:05 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/20 07:38:47 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include "annotations.h"
# include "../utils/time.h"
# include "../utils/ft/ft.h"
# include "../io/print.h"
# include "usb.h"
# include "logger.h"
# include "codexion.h"

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>

typedef enum e_coder_state
{
	COMPILE,
	DEBUG,
	REFACTOR,
}	t_coder_state;

typedef struct s_coder
{
	int							__id;
	t_logger *NULLABLE			__logger;
	pthread_t NULL_UNSPECIFIED	___thread;
	bool						__thread_active;
	bool						__exit_thread;
	t_usb *NULLABLE				__left_usb;
	t_usb *NULLABLE				__right_usb;
	t_coder_state				__state;
	int							__compiles;
	t_ms						__last_compile;
	int							__time_to_compile;
	int							__time_to_debug;
	int							__time_to_refactor;
	int							__number_of_compiles_required;

}	t_coder;

int					coder__init(
						t_coder *NONNULL self,
						int id,
						t_config *NONNULL config,
						t_logger *NULLABLE logger
						);
t_coder *NULLABLE	coder__create(
						int id,
						t_config *NULLABLE config,
						t_logger *NULLABLE logger
						);
void				coder__reset(t_coder *NONNULL self);
void				coder__destroy(t_coder *NONNULL coder);

/* GETTERS*/

static inline t_usb	*NULLABLE	coder__left_usb(const t_coder *NONNULL self)
{
	return (self->__left_usb);
}

static inline t_usb	*NULLABLE	coder__right_usb(const t_coder *NONNULL self)
{
	return (self->__right_usb);
}

static inline t_coder_state	coder__state(const t_coder *NONNULL self)
{
	return (self->__state);
}

/* SETTERS */
void				coder__set_left_usb(
						t_coder *NONNULL self,
						const t_usb *NULLABLE usb
						);
void				coder__set_right_usb(
						t_coder *NONNULL self,
						const t_usb *NULLABLE usb
						);
void				coder__set_exit_thread(
						t_coder *NONNULL self,
						const bool exit_thread
						);

/* THREAD LIFECYCLE */
int					coder__init_thread(t_coder *NONNULL self);
int					coder__join_thread(t_coder *NONNULL self);
int					coder__exit_thread(t_coder *NONNULL self);

/* THREAD */
void *NULLABLE		coder__th_start_routine(t_coder *NONNULL self);

#endif /* CODER_H */