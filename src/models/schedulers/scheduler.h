/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:19:30 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/25 06:59:24 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "annotations.h"

/* > 1 < INT_MAX */
# define MAX_CODERS 2

typedef struct s_coder		t_coder;
typedef t_coder *NULLABLE	t_coder_p;
typedef t_coder *NONNULL	t_coder_nn;

typedef int					(*t_put_fn)(void *NULLABLE self, t_coder_nn coder);
typedef void				(*t_pop_fn)(void *NULLABLE self, t_coder_nn coder);
typedef void				(*t_del_fn)(void *NULLABLE self, t_coder_nn coder);
typedef t_coder_p			(*t_first_fn)(void *NULLABLE self);
typedef void				(*t_reset_fn)(void *NULLABLE self);
typedef void				(*t_destroy_fn)(void *NULLABLE self);

typedef struct s_scheduler
{
	void *NULLABLE				__self;
	t_put_fn NULLABLE			__put;
	t_pop_fn NULLABLE			__pop;
	t_del_fn NULLABLE			__delete;
	t_first_fn NULLABLE			__first;
	t_reset_fn NULLABLE			__reset;
	t_destroy_fn NULLABLE		__destroy;
}	t_scheduler;

/* GETTERS */
static inline void *NULLABLE	scheduler__self(t_scheduler *NONNULL self)
{
	return (self->__self);
}

static inline int	scheduler__put(
	t_scheduler *NONNULL self,
	t_coder *NONNULL coder
)
{
	return (self->__put(self->__self, coder));
}

static inline void	scheduler__pop(
	t_scheduler *NONNULL self,
	t_coder *NONNULL coder
)
{
	self->__pop(self->__self, coder);
}

static inline void	scheduler__delete(
	t_scheduler *NONNULL self,
	t_coder *NONNULL coder
)
{
	self->__delete(self->__self, coder);
}

#endif /* SCHEDULER_H */
