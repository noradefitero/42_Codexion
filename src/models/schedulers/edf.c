/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 01:37:56 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 11:46:55 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "edf.h"

void	edf__init(t_edf *NONNULL self)
{
	self->super.__self = self;
	self->super.__put = (int (*)(void *, t_coder *))edf__put;
	self->super.__pop = (void (*)(void *, t_coder *))edf__pop;
	self->super.__delete = (void (*)(void *, t_coder *))edf__delete;
	self->super.__first = (t_coder *(*)(void *))edf__first;
	self->super.__reset = (void (*)(void *))edf__reset;
	self->super.__destroy = (void (*)(void *))edf__destroy;
	self->__head = 0;
	self->__tail = 0;
	self->__size = 0;
	memset(self->__queue, 0, sizeof(self->__queue));
}

t_edf *NULLABLE	edf__create(void)
{
	t_edf	*result;

	result = (t_edf *)ft_calloc(1, sizeof(t_edf));
	if (!result)
	{
		print_error("FAILED ALLOCATING AN EDF SCHEDULER INSTANCE", false);
		return (NULL);
	}
	edf__init(result);
	return (result);
}

void	edf__reset(t_edf *NONNULL self)
{
	self->__head = 0;
	self->__tail = 0;
	self->__size = 0;
}

void	edf__destroy(t_edf *NONNULL edf)
{
	if (edf)
	{
		edf__reset(edf);
		free(edf);
	}
}
