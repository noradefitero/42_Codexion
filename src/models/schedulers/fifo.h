/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:36:50 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/11 05:44:32 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FIFO_H
# define FIFO_H

# include "scheduler.h"
# include "../../utils/ft/ft.h"
# include "../../io/print.h"

# include <pthread.h>
# include <stdbool.h>
# include <string.h>

typedef struct s_fifo
{
	t_scheduler			super;
	t_coder *NULLABLE	__queue[MAX_CODERS];
	int					__head;
	int					__tail;
	int					__size;
}	t_fifo;

void				fifo__init(t_fifo *NONNULL self);
t_fifo *NULLABLE		fifo__create(void);
void				fifo__reset(t_fifo *NONNULL self);
void				fifo__destroy(t_fifo *NONNULL fifo);

/* GETTERS */
static inline t_coder *NULLABLE	fifo__first(t_fifo *NONNULL self)
{
	if (self->__size < 1)
		return (NULL);
	return (self->__queue[self->__head]);
}

int					fifo__put(
						t_fifo *NONNULL self,
						t_coder *NONNULL coder
						);
void				fifo__pop(
						t_fifo *NONNULL self,
						t_coder *NONNULL coder
						);
void				fifo__delete(t_fifo *NONNULL self, t_coder *NONNULL coder);

#endif /* FIFO_H */