/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   capture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:41:10 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:00 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "capture.h"

#define CAP_CHUNK 4096

static void	*reader_thread(void *arg)
{
	t_capture	*cap;
	ssize_t		n;
	char		tmp[CAP_CHUNK];

	cap = (t_capture *)arg;
	while ((n = read(cap->pipefd[0], tmp, sizeof(tmp))) > 0)
	{
		if (cap->len + (size_t)n + 1 > cap->cap)
		{
			cap->cap = cap->len + (size_t)n + 1;
			cap->buf = (char *)realloc(cap->buf, cap->cap);
		}
		memcpy(cap->buf + cap->len, tmp, (size_t)n);
		cap->len += (size_t)n;
	}
	return (NULL);
}

int	capture_begin(int fd, t_capture *cap)
{
	cap->fd = fd;
	cap->active = false;
	cap->buf = NULL;
	cap->len = 0;
	cap->cap = 0;
	cap->saved = dup(fd);
	if (cap->saved < 0)
		return (-1);
	if (pipe(cap->pipefd) != 0)
	{
		close(cap->saved);
		return (-1);
	}
	if (dup2(cap->pipefd[1], fd) < 0)
	{
		close(cap->saved);
		close(cap->pipefd[0]);
		close(cap->pipefd[1]);
		return (-1);
	}
	cap->active = true;
	if (pthread_create((pthread_t *)&cap->thread, NULL, reader_thread, cap) != 0)
	{
		dup2(cap->saved, fd);
		close(cap->saved);
		close(cap->pipefd[0]);
		close(cap->pipefd[1]);
		cap->active = false;
		return (-1);
	}
	return (0);
}

char	*capture_end(t_capture *cap)
{
	char	*result;

	if (!cap->active)
		return (strdup(""));
	fflush(NULL);
	dup2(cap->saved, cap->fd);
	close(cap->saved);
	close(cap->pipefd[1]);
	pthread_join((pthread_t)cap->thread, NULL);
	close(cap->pipefd[0]);
	cap->active = false;
	if (!cap->buf)
		return (strdup(""));
	if (cap->len + 1 > cap->cap)
	{
		cap->cap = cap->len + 1;
		cap->buf = (char *)realloc(cap->buf, cap->cap);
	}
	cap->buf[cap->len] = '\0';
	result = cap->buf;
	cap->buf = NULL;
	cap->len = 0;
	cap->cap = 0;
	return (result);
}

bool	str_contains(const char *haystack, const char *needle)
{
	if (!haystack || !needle)
		return (false);
	return (strstr(haystack, needle) != NULL);
}
