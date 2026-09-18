/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_models.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:43:26 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:27 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"

void	run_coder_tests(void);
void	run_monitor_tests(void);
void	run_hub_tests(void);

int	main(void)
{
	UNITY_BEGIN();
	run_coder_tests();
	run_monitor_tests();
	run_hub_tests();
	return (UNITY_END());
}
