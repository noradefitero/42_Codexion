/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 06:05:23 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/26 13:00:16 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USB_H
# define USB_H

# include "schedulers/fifo.h"
# include "schedulers/edf.h"
# include "schedulers/scheduler.h"
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

static inline int	usb__active(t_usb *NONNULL self)
{
	return (self->__active);
}

static inline t_coder *NULLABLE	usb__first(t_usb *NONNULL self)
{
	t_coder	*ret;

	pthread_mutex_lock(&self->__mutex);
	ret = scheduler__first(self->__scheduler);
	pthread_mutex_unlock(&self->__mutex);
	return (ret);
}

/* Requires the usb mutex to be already held by the caller. */
t_coder *NULLABLE			usb__first_unsafe(t_usb *NONNULL self);
void						usb__wake(t_usb *NONNULL self);

static inline t_scheduler *NONNULL	usb__scheduler_unsafe(t_usb *NONNULL self)
{
	return (self->__scheduler);
}

/*
* Returns the milliseconds remaining until the dongle cooldown expires,
* or 0 if there is no active cooldown. The caller must NOT hold the usb
* mutex unless it is the same thread that owns the wait (since this only
* reads fields, no lock is required for the read itself).
*/
static inline t_ms	usb__cooldown_remaining(const t_usb *NONNULL self)
{
	t_ms	now;
	t_ms	deadline;

	if (self->__dongle_cooldown <= 0 || self->__last_used < 0)
		return (0);
	deadline = self->__last_used + (t_ms)self->__dongle_cooldown;
	now = get_time();
	if (now >= deadline)
		return (0);
	return (deadline - now);
}

int					usb__acquire(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__release(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);
void				usb__delete(
						t_usb *NONNULL self,
						t_coder *NONNULL coder
						);

#endif /* USB_H */