/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_cmd.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 22:31:54 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/30 19:45:45 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	norm_is_last1(t_node **node)
{
	if (((*node)->shell->lvl_subshell - 1) > 0)
	{
		if (!next_cmd_same_lvl(*node) || ft_back_node(next_cmd_same_lvl(
					*node))->lvl_subshell <= (*node)->shell->lvl_subshell)
			return ((*node)->shell->lvl_subshell);
		return ((*node)->shell->lvl_subshell - 1);
	}
	else if ((*node)->shell->lvl_subshell - 1 == 0)
	{
		if ((*node)->shell->lvl_subshell == 1 && (!next_cmd_same_lvl(*node)
				|| ft_back_node(next_cmd_same_lvl(
						*node))->lvl_subshell <= 1))
			return ((*node)->shell->lvl_subshell);
		else
			return ((*node)->shell->lvl_subshell + 1);
	}
	return (0);
}

int	get_max_lvl(t_node *node, t_node *stg)
{
	if (node->shell->lvl_subshell == 0 || !stg->content.redir)
		return (0);
	if (ft_back_node(stg)->lvl_subshell > ft_back_node(node)->lvl_subshell)
		return (ft_back_node(node)->lvl_subshell + 1);
	if (ft_back_node(stg)->lvl_subshell == ft_back_node(node)->lvl_subshell
		&& stg->is_last != 1)
	{
		if (ft_back_node(node)->lvl_subshell - 1 > 0)
			return (ft_back_node(node)->lvl_subshell - 1);
		else
			return (ft_back_node(node)->lvl_subshell);
	}
	if (stg->is_last == 1)
		return (norm_is_last1(&node));
	return (0);
}

int	ft_do_redir2_pipe(t_node *node)
{
	int		flag;
	t_node	*storage;

	flag = 0;
	storage = fd_storage(node);
	if (storage->content.redir && ft_fd_sub_level2(storage,
			node->shell->lvl_subshell, &flag) == 0)
	{
		ft_reset_original_fd(node);
		return (norm_exit_status(node, 2));
	}
	return (norm_exit_status(node, 0));
}

// V2 versione per single_cmd
int	ft_do_redir3(t_node *node)
{
	int	flag;
	int	max_lvl;

	flag = 0;
	if (node->content.redir)
		max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
	else
		max_lvl = 0;
	while (max_lvl > 0)
	{
		if (ft_fd_sub_level2(node, max_lvl--, &flag) == 0)
		{
			ft_reset_original_fd(node);
			return (norm_exit_status(node, 2));
		}
	}
	if (ft_fd_cmd_level(node) == 1)
	{
		ft_reset_original_fd(node);
		return (norm_exit_status(node, 1));
	}
	return (norm_exit_status(node, 0));
}

void	norm_else_single_cmd(t_node **node, int *status, int pid)
{
	waitpid(pid, status, 0);
	(*node)->shell->exit_status = WSTOPSIG(*status);
}
