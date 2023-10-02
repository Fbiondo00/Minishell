/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 01:29:36 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/10/02 17:02:02 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ftcheck11(t_node *node, int i)
{
	int	y;

	y = -1;
	if (node->content.cmd[i])
	{
		while (node->content.cmd[i][++y])
		{
			if ((node->content.cmd[i][y] >= '0'
					&& node->content.cmd[i][y] <= '9')
				|| (node->content.cmd[i][y] >= 'a'
					&& node->content.cmd[i][y] <= 'z')
				|| (node->content.cmd[i][y] >= 'A'
					&& node->content.cmd[i][y] <= 'Z')
				|| node->content.cmd[i][y] == '='
				|| node->content.cmd[i][y] == '+')
				continue ;
			else
				return (1);
		}
	}
	return (0);
}

int	ft_check(t_node *node, int i)
{
	if (node->content.cmd[i])
	{
		if ((node->content.cmd[i][0] >= 'a' && node->content.cmd[i][0] <= 'z')
			|| (node->content.cmd[i][0] >= 'A'
				&& node->content.cmd[i][0] <= 'Z'))
			;
		else
			return (write(2, "not a valid identifier\n", 23) - 22);
	}
	if (ftcheck11(node, i) == 1)
		return (write(2, "not a valid identifier\n", 23) - 22);
	return (0);
}

int	ft_controll(t_node *node, t_shell *shell, int y, int ij)
{
	if (ft_check(node, y) == 1)
	{
		if (node->content.cmd[++y])
			ft_name_value(node, shell, ij, y);
		return (1);
	}
	return (0);
}

void	ft_name_value(t_node *node, t_shell *shell, int ij, int y)
{
	int	i;
	int	j;

	j = ft_get_len_env(shell);
	i = 0;
	while (node->content.cmd[y][++i])
		;
	if (ft_controll(node, shell, y, ij) == 1)
		return ;
	if (ij > y)
	{
		if ((ft_reach(shell, node->content.cmd[y]) == 1))
		{
			if (node->content.cmd[y + 1])
				ft_name_value(node, shell, ij, ++y);
			return ;
		}
		shell->env = ft_realloc(shell->env, sizeof(char *) * (j + 2));
		shell->env[j] = ft_strdup(node->content.cmd[y]);
		shell->env[j + 1] = 0;
	}
	if (ij - 1 == y)
		return ;
	else
		ft_name_value(node, shell, ij, ++y);
}
