/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   annotations.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 07:35:41 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 03:33:37 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ANNOTATIONS_H
# define ANNOTATIONS_H

# if defined(__clang__)
#  define NULLABLE __nullable
#  define NONNULL __nonnull
#  define NULL_UNSPECIFIED __null_unspecified
# else
#  define NULLABLE
#  define NONNULL
#  define NULL_UNSPECIFIED
# endif

#endif /* ANNOTATIONS_H */
