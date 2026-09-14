/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 06:05:23 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/10 23:35:57 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USB_H
# define USB_H

# include "schedulers/fifo.h"
# include "schedulers/edf.h"
# include "schedulers/scheduler.h"
# include "schedulers/scheduler_getters.h"
# include "codexion.h"
# include "annotations.h"
# include "../utils/ft/ft.h"
# include "../io/print.h"

# include <pthread.h>
# include <stdbool.h>

typedef struct s_usb
{
	t_scheduler *NULLABLE	__scheduler;
	pthread_mutex_t			__mutex;
	bool					__mutex_initialized;
	pthread_cond_t			__cond;
	bool					__cond_initialized;
	int						__dongle_cooldown;
	t_ms					__last_used;
	bool					__active;
}	t_usb;

int					usb__init(
						t_usb *NONNULL self,
						t_config *NONNULL config
						);
t_usb *NULLABLE		usb__create(
						t_config *NONNULL config
						);
void				usb__reset(t_usb *NONNULL self);
void				usb__destroy(t_usb *NONNULL usb);

/* GETTERS */
static inline pthread_mutex_t *NONNULL	usb__mutex(const t_usb *NONNULL self)
{
	return ((pthread_mutex_t *)&self->__mutex);
}

static inline pthread_cond_t *NONNULL	usb__cond(const t_usb *NONNULL self)
{
	return ((pthread_cond_t *)&self->__cond);
}

static inline t_ms	usb__cooldown_deadline(const t_usb *NONNULL self)
{
	if (self->__dongle_cooldown <= 0)
		return (0);
	return (self->__last_used + (t_ms)self->__dongle_cooldown);
}

static inline t_coder *NULLABLE	usb__first(t_usb *NONNULL self)
{
	return (scheduler__first(self->__scheduler));
}

int					usb__acquire(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__delete(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__release_safe(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__delete_safe(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__wake_safe(t_usb *NONNULL self);

#endif /* USB_H */