/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usb.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 06:05:23 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/17 08:07:09 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USB_H
# define USB_H

# include "codexion.h"
# include "annotations.h"
# include "../utils/ft/ft.h"

# include <pthread.h>
# include <stdbool.h>

typedef struct s_usb
{
	pthread_mutex_t	__mutex;
	bool			__mutex_initialized;
	pthread_cond_t	__cond;
	bool			__cond_initialized;
	t_ms			*__initial_time;
	int				__dongle_cooldown;
	t_scheduler		__scheduler;
}	t_usb;

int				usb__init(
					t_usb *NONNULL self,
					t_config *NONNULL config,
					int *NONNULL initial_time
					);
t_usb *NULLABLE	usb__create(
					t_config *NONNULL config,
					int *NONNULL initial_time
					);
void			usb__reset(t_usb *NONNULL self);
void			usb__destroy(t_usb *NONNULL usb);

/* GETTERS */
pthread_mutex_t	usb__mutex(const t_usb *NONNULL self);

#endif /* USB_H */