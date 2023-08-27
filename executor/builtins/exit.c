/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:28 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:32:32 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_isdigit(int n)
{
	if ((n < 48 || n > 57))
		return (0);
	return (1);
}

int	is_check(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
	}
	return (1);
}

void	free_all2_0(t_shell *shell, char *str, int exit_status, int to_exit)
{
	ft_clean_exit(shell, str, exit_status, to_exit);
}

void	ft_exit_2(t_node *node, t_shell *shell, int i)
{
	int	y;

	if (!is_check(node->content.cmd[i - 1]))
	{
		free_all2_0(shell, "numeric argument required\n", 255, 1);
	}
	else
	{
		y = ft_atoi(node->content.cmd[i - 1]);
		while (y >= 256)
			y -= 256;
		while (y < 0)
			y += 256;
		free_all2_0(shell, "exit\n", y, 1);
	}
}

int	ft_exit(t_node *node, t_shell *shell)
{
	int	i;

	i = -1;
	while (node->content.cmd[++i])
		;
	printf("exit:i:%d\n", i);
	if (i == 1)
		free_all2_0(shell, "exit\n", 0, 1);
	else
	{
		if (i > 3)
			free_all2_0(shell, "exit: too many arguments\n", 0, 1);
		ft_exit_2(node, shell, i);
	}
	return (0);
}
