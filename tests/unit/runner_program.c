/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_program.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:43:29 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:30 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"

void	run_binary_tests(void);

int	main(void)
{
	UNITY_BEGIN();
	run_binary_tests();
	return (UNITY_END());
}
