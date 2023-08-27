/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:18 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 03:23:43 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	ft_ric2(int i, t_shell *shell)
{
	while (shell->env[i + 1])
	{
		swap_(&shell->env[i], &shell->env[i + 1]);
		i++;
	}
	shell->env[i] = 0;
}

void	ft_ricorsione(t_node *node, t_shell *shell, int ij, int y)
{
	int	i;
	int	j;

	i = -1;
	while (shell->env[++i])
	{
		j = -1;
		while (shell->env[i][++j] != '=' && shell->env[i][j])
			;
		if (((ft_strncmp(shell->env[i], node->content.cmd[y], j, 0)) == 0))
		{
			if (((ft_strncmp(shell->env[i], node->content.cmd[y],
							ft_strlen(node->content.cmd[y]), 0)) == 0))
			{
				free(shell->env[i]);
				break ;
			}
		}
	}
	ft_ric2(i, shell);
	if (node->content.cmd[y + 1])
		ft_ricorsione(node, shell, ij, ++y);
}

int	ft_check12(t_node *node)
{
	int	i;
	int	y;

	i = 0;
	y = -1;
	while (node->content.cmd[++i])
	{
		while (node->content.cmd[i][++y])
		{
			if ((node->content.cmd[i][y] >= '0'
				&& node->content.cmd[i][y] <= '9')
				|| (node->content.cmd[i][y] >= 'a'
					&& node->content.cmd[i][y] <= 'z')
				|| (node->content.cmd[i][y] >= 'A'
					&& node->content.cmd[i][y] <= 'Z'))
				continue ;
			else
				return (1);
		}
	}
	return (0);
}

int	ft_check_unset(t_node *node)
{
	int	i;

	i = 1;
	while (node->content.cmd[i])
	{
		if ((node->content.cmd[i][0] >= 'a' && node->content.cmd[i][0] <= 'z')
			|| (node->content.cmd[i][0] >= 'A'
				&& node->content.cmd[i][0] <= 'Z'))
			++i;
		else
			return (write(1, "checkexpo\n", 11) - 10);
	}
	if (ft_check12(node) == 1)
		return (write(1, "checkexpo\n", 11) - 10);
	return (0);
}

void	ft_unset(t_node *node, t_shell *shell)
{
	int	ij;

	ij = ft_get_len_mat(node);
	if (!node->content.cmd[1])
		return ;
	if (ij >= 2)
	{
		if (ft_check_unset(node) == 1)
			return ;
		ft_ricorsione(node, shell, ij, 1);
	}
}
