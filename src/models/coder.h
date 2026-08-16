/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 03:19:05 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 22:21:13 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include "annotations.h"
# include "../utils/ft/ft.h"
# include "usb.h"

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>

typedef enum e_coder_state
{
	COMPILE,
	DEBUG,
	REFACTOR,
	UNDEFINED
}	t_coder_state;

typedef struct s_coder
{
	pthread_t NULL_UNSPECIFIED	___thread;
	bool						__thread_initialized;
	bool						__exit_thread;
	t_usb *NULLABLE				__left_usb;
	t_usb *NULLABLE				__right_usb;
	t_coder_state				__state;
}	t_coder;

int					coder__init(t_coder *NONNULL self);
t_coder *NULLABLE	coder__create(void);
void				coder__reset(t_coder *NONNULL self);
void				coder__destroy(t_coder *NONNULL coder);

/* GETTERS*/

t_usb *NULLABLE		coder__left_usb(const t_coder *NONNULL self);
t_usb *NULLABLE		coder__right_usb(const t_coder *NONNULL self);
t_coder_state		coder__state(const t_coder *NONNULL self);

/* SETTERS */
void				coder__set_left_usb(
						t_coder *NONNULL self,
						t_usb *NULLABLE usb
						);
void				coder__set_right_usb(
						t_coder *NONNULL self,
						t_usb *NULLABLE usb
						);
void				coder__set_exit_thread(
						t_coder *NONNULL self,
						bool exit_thread
						);

/* THREAD LIFECYCLE */
int					coder__init_thread(t_coder *NONNULL self);
int					coder__join_thread(t_coder *NONNULL self);
int					coder__exit_thread(t_coder *NONNULL self);

/* THREAD */
void *NULLABLE		coder__th_start_routine(t_coder *NONNULL self);

#endif /* CODER_H */