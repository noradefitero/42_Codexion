/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:43:34 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:35 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"

void	run_ft_tests(void);
void	run_time_tests(void);

int	main(void)
{
	UNITY_BEGIN();
	run_ft_tests();
	run_time_tests();
	return (UNITY_END());
}
