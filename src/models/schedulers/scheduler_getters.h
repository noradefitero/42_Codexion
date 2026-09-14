/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_getters.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 12:00:00 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 12:00:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_GETTERS_H
# define SCHEDULER_GETTERS_H

# include "scheduler.h"

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

#endif /* SCHEDULER_GETTERS_H */
