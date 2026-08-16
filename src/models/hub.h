/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 03:22:46 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 22:14:19 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUB_H
# define HUB_H

# include "annotations.h"
# include "coder.h"
# include "usb.h"
# include "../utils/ft/ft.h"

typedef struct s_hub
{
	t_coder *NULLABLE *NULLABLE	__coders; /* Array of coders [*1, *2, NULL] */
	t_usb *NULLABLE *NULLABLE	__usbs;
	size_t						__n_coders;
}	t_hub;

int							hub__init(t_hub *NONNULL self, size_t n_coders);
t_hub *NULLABLE				hub__create(size_t n_coders);
void						hub__reset(t_hub *NONNULL self);
void						hub__destroy(t_hub *NONNULL hub);

/* LIFECYCLE */
t_usb *NONNULL *NULLABLE		hub__create_usbs(size_t n_usbs);
t_coder *NONNULL *NULLABLE	hub__create_coders(
								size_t n_coders,
								t_usb *NULLABLE *NONNULL usbs
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

/* GETTERS */
t_coder *NULLABLE *NULLABLE	hub__coders(const t_hub *NONNULL self);
t_usb *NULLABLE *NULLABLE		hub__usbs(const t_hub *NONNULL self);
size_t						hub__n_coders(const t_hub *NONNULL self);

#endif /* HUB_H */