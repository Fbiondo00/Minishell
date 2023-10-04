/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/10/04 13:31:42 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_flag_status = 0;

int	in_quotes(t_node *node, int index)
{
	int	i;
	int	d_quotes;
	int	s_quotes;

	i = -1;
	d_quotes = 0;
	s_quotes = 0;
	while (++i < index)
	{
		if (node->quote_idx[i] == 34)
			d_quotes++;
		if (node->quote_idx[i] == 39)
			s_quotes++;
	}
	if (s_quotes % 2 != 0)
		return (-1);
	if (d_quotes % 2 != 0)
		return (1);
	return (0);
}

int	main(int argc, char **argv, char **env)
{
	t_shell	shell;

	if (argc != 1)
		exit(write(2, "Error: invalid arguments\n", 25) - 24);
	shell_init(argc, argv, env, &shell);
	while (1)
	{
		ft_read_line(&shell);
		if (shell.rawline && shell.rawline[0])
		{
			set_tree(&shell);
			set_components(&shell);
			executev2(&shell);
			ft_clean_exit(&shell, NULL, shell.exit_status, 0);
		}
	}
}
