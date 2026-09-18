/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   capture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:42:07 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:12 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * stdout/stderr capture helpers for Unity tests
*/

#ifndef CAPTURE_H
# define CAPTURE_H

# include <stdbool.h>
# include <stddef.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef struct s_capture
{
	int					fd;
	int					saved;
	int					pipefd[2];
	void				*thread;
	bool				active;
	char				*buf;
	size_t				len;
	size_t				cap;
}	t_capture;

/*
* Begins capturing writes to `fd`. Must be paired with capture_end().
* Returns 0 on success, -1 on failure (nothing captured).
*/
int			capture_begin(int fd, t_capture *cap);

/*
* Stops capturing, restores the original fd and returns a malloc'd
* NUL-terminated string with everything written since capture_begin().
* The caller owns the returned string (free it). Returns "" if nothing
* was captured. Never returns NULL.
*/
char		*capture_end(t_capture *cap);

/* Convenience: returns true if `haystack` contains `needle`. */
bool		str_contains(const char *haystack, const char *needle);

#endif /* CAPTURE_H */
