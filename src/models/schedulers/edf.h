/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 09:36:50 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/14 11:33:09 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EDF_H
# define EDF_H

# include "scheduler.h"
# include "../../utils/ft/ft.h"
# include "../../io/print.h"

# include <pthread.h>
# include <stdbool.h>
# include <string.h>

typedef struct s_edf
{
	t_scheduler			super;
	t_coder *NULLABLE	__queue[MAX_CODERS];
	int					__head;
	int					__tail;
	int					__size;
}	t_edf;

void				edf__init(t_edf *NONNULL self);
t_edf *NULLABLE		edf__create(void);
void				edf__reset(t_edf *NONNULL self);
void				edf__destroy(t_edf *NONNULL edf);
int					edf__at(t_edf *NONNULL self, int index);
void				edf__sift_up(t_edf *NONNULL self, int index);
void				edf__sift_down(t_edf *NONNULL self, int index);

/* GETTERS */
static inline t_coder *NULLABLE	edf__first(t_edf *NONNULL self)
{
	if (self->__size < 1)
		return (NULL);
	return (self->__queue[self->__head]);
}

int					edf__put(
						t_edf *NONNULL self,
						t_coder *NONNULL coder
						);
void				edf__pop(
						t_edf *NONNULL self,
						t_coder *NONNULL coder
						);
void				edf__delete(t_edf *NONNULL self, t_coder *NONNULL coder);

#endif /* EDF_H */
