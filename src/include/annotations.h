/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   annotations.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 07:35:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/08/16 07:39:08 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ANNOTATIONS_H
# define ANNOTATIONS_H

# ifdef __nullable
#  define NULLABLE __nullable
# else
#  define NULLABLE
# endif

# ifdef __nonnull
#  define NONNULL __nonnull
# else
#  define NONNULL
# endif

# ifdef __null_unspecified
#  define NULL_UNSPECIFIED __null_unspecified
# else
#  define NULL_UNSPECIFIED
# endif

#endif /* ANNOTATIONS_H */