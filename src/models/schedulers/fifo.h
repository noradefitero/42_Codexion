/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:36:50 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 00:04:25 by dde-fite         ###   ########.fr       */
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
}	t_fifo;

void				fifo__init(t_fifo *NONNULL self);
t_fifo *NULLABLE		fifo__create(void);
void				fifo__reset(t_fifo *NONNULL self);
void				fifo__destroy(t_fifo *NONNULL fifo);

/* GETTERS */
static inline t_coder *NULLABLE	fifo__first(t_fifo *NONNULL self)
{
	return (self->__queue[0]);
}

int					fifo__acquire(
						t_fifo *NONNULL self,
						t_coder *NONNULL coder
						);
void				fifo__release(
						t_fifo *NONNULL self,
						t_coder *NONNULL coder
						);

#endif /* FIFO_H */