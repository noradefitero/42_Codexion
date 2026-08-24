/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_th_lifecycle.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:06:47 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/24 07:08:17 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

int	coder__init_thread(t_coder *NONNULL self)
{
	if (pthread_create(
			&self->___thread,
			NULL,
			(void *NULLABLE (*NONNULL)(void *NONNULL))coder__th_start_routine,
		self
	)
	)
		return (print_error("FAILED CREATING CODER THREAD", false));
	self->__thread_active = true;
	return (0);
}

int	coder__join_thread(t_coder *NONNULL self)
{
	return (pthread_join(self->___thread, NULL));
}

int	coder__exit_thread(t_coder *NONNULL self)
{
	if (self->__thread_active)
		self->__exit_thread = true;
	return (0);
}
