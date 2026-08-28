/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:36:50 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 06:19:54 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EDF_H
# define EDF_H

# include "scheduler.h"
# include "../../utils/ft/ft.h"
# include "../../io/print.h"

# include <pthread.h>
# include <stdbool.h>

typedef struct s_edf_node
{
	t_coder *NONNULL				content;
	struct s_edf_node *NULLABLE	next;
}	t_edf_node;

typedef struct s_edf
{
	t_scheduler				super;
	t_edf_node *NULLABLE	__queue;
	t_edf_node *NULLABLE	__queue_tail;
	bool					__active;
}	t_edf;

void				edf__init(t_edf *NONNULL self);
t_edf *NULLABLE		edf__create(void);
void				edf__reset(t_edf *NONNULL self);
void				edf__destroy(t_edf *NONNULL edf);

/* GETTERS */
static inline t_coder *NULLABLE	edf__first(t_edf *NONNULL self)
{
	if (self->__queue == NULL)
		return (NULL);
	return (self->__queue->content);
}

int					edf__put(
						t_edf *NONNULL self,
						t_coder *NONNULL coder
						);
void				edf__pop(
						t_edf *NONNULL self,
						t_coder *NONNULL coder
						);
void				edf__clear_queue(t_edf *NONNULL self);

#endif /* EDF_H */