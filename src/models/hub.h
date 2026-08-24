/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 03:22:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 06:46:14 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUB_H
# define HUB_H

# include "annotations.h"
# include "coder.h"
# include "usb.h"
# include "monitor.h"
# include "logger.h"
# include "../utils/ft/ft.h"

typedef struct s_hub
{
	t_config					__config;
	t_coder *NULLABLE *NULLABLE	__coders; /* Array of coders [*1, *2, NULL] */
	t_usb *NULLABLE *NULLABLE	__usbs;
	t_monitor					__monitor;
	bool						__monitor_init;
	t_logger					__logger;
	bool						__logger_init;
}	t_hub;

int							hub__init(
								t_hub *NONNULL self,
								t_config *NONNULL config
								);
t_hub *NULLABLE				hub__create(t_config *NONNULL config);
void						hub__reset(t_hub *NONNULL self);
void						hub__destroy(t_hub *NONNULL hub);

/* GETTERS */

static inline t_coder *NULLABLE *NULLABLE	hub__coders(
	const t_hub *NONNULL self
)
{
	return (self->__coders);
}

static inline t_usb *NULLABLE *NULLABLE	hub__usbs(const t_hub *NONNULL self)
{
	return (self->__usbs);
}

static inline t_monitor *NONNULL	hub__monitor(const t_hub *NONNULL self)
{
	return ((t_monitor *)&self->__monitor);
}

static inline t_logger *NONNULL	hub__logger(const t_hub *NONNULL self)
{
	return ((t_logger *)&self->__logger);
}

/* LIFECYCLE */
int							hub__create_usbs(t_hub *NONNULL self);
int							hub__create_coders(
								t_hub *NONNULL self
								);
void						hub__destroy_usbs(
								t_usb *NONNULL *NONNULL usbs,
								size_t n_usbs
								);
void						hub__destroy_coders(
								t_coder *NULLABLE *NONNULL coders,
								size_t n_coders
								);
int							hub__coders_map(
								t_hub *NONNULL self,
								int (*NONNULL f)(t_coder *NONNULL)
								);

/* EVENTS */
void						hub__on_burn(t_hub *NONNULL self);

#endif /* HUB_H */