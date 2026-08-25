/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:19:30 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 23:12:19 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
# define SCHEDULER_H

# include "annotations.h"

/* >= 2 */
# define MAX_CODERS 2

typedef struct s_coder	t_coder;

typedef struct s_scheduler
{
	void *NULLABLE		__self;
	int					(*NULLABLE __acquire)(void *NULLABLE self, t_coder *NONNULL fifo);
	void				(*NULLABLE __release)(void *NULLABLE self, t_coder *NONNULL fifo);
	t_coder *NULLABLE	(*NULLABLE __first)(void *NULLABLE self);
	void				(*NULLABLE __reset)(void *NULLABLE self);
	void				(*NULLABLE __destroy)(void *NULLABLE self);

}	t_scheduler;

/* GETTERS */
static inline void *NULLABLE	scheduler__self(t_scheduler *NONNULL self)
{
	return (self->__self);
}

static inline int	scheduler__acquire(
	t_scheduler *NONNULL self,
	t_coder *NONNULL coder
)
{
	return (self->__acquire(self->__self, coder));
}

static inline void	scheduler__release(
	t_scheduler *NONNULL self,
	t_coder *NONNULL coder
)
{
	self->__release(self->__self, coder);
}

static inline t_coder *NULLABLE	scheduler__first(
	t_scheduler *NONNULL self
)
{
	return (self->__first(self->__self));
}

static inline void	scheduler__reset(
	t_scheduler *NONNULL self
)
{
	self->__reset(self->__self);
}

static inline void	scheduler__destroy(
	t_scheduler *NONNULL self
)
{
	self->__destroy(self->__self);
}

#endif /* SCHEDULER_H */