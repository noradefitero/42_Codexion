/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 06:59:43 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 01:22:06 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

int	coder__init(t_coder *self)
{
	self->__state = UNDEFINED;
	return (0);
}

t_coder	*coder__create(void)
{
	t_coder	*result;

	result = (t_coder *)ft_calloc(1, sizeof(t_coder));
	if (!result)
		return (NULL);
	if (coder__init(result))
	{
		coder__destroy(result);
		return (NULL);
	}
	return (result);
}

void	coder__reset(t_coder *self)
{
	self->__state = self->__state;
}

void	coder__destroy(t_coder *coder)
{
	if (coder)
	{
		coder__reset(coder);
		free(coder);
	}
}
