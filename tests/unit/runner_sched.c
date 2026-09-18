/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_sched.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:41:06 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:33 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"

void	run_fifo_tests(void);
void	run_edf_tests(void);
void	run_usb_tests(void);

int	main(void)
{
	UNITY_BEGIN();
	run_fifo_tests();
	run_edf_tests();
	run_usb_tests();
	return (UNITY_END());
}
