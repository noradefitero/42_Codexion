/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runner_io.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:43:24 by dde-fite          #+#    #+#             */
/*   Updated: 2026/09/17 19:43:25 by dde-fite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"

void	run_parsing_tests(void);
void	run_print_tests(void);
void	run_commands_tests(void);
void	run_logger_tests(void);

int	main(void)
{
	UNITY_BEGIN();
	run_parsing_tests();
	run_print_tests();
	run_commands_tests();
	run_logger_tests();
	return (UNITY_END());
}
