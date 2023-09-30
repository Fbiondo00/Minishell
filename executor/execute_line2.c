/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 02:03:01 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/29 19:04:56 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_node	*wrap_ft_single_cmd2(t_node *node)
{
	int	res;

	res = ft_single_cmd2(node, ft_do_redir2);
	if (res == 1)
		return (next_cmd_same_lvl(node));
	if (res == 2)
		ft_clean_exit(node->shell, NULL, 34, 1);
	return (next_cmd_same_lvl(node));
}

int	norm_if_exec_recursive(t_node **node, t_node **temp)
{
	if ((*node) == NULL || (*node)->done_lock == 1 || (*node
			&& ft_back_node(*node)->lvl_subshell < (*node)->shell->lvl_subshell
			&& (*node)->shell->lvl_subshell != 0))
	{
		if ((*node) == NULL)
		{
			while ((*temp)->shell->lvl_subshell > 0)
				ft_clean_exit((*temp)->shell, NULL, (*temp)
					->shell->exit_status, 1);
			if ((*temp)->shell->lvl_subshell == 0)
				return (1);
		}
		else
			ft_clean_exit((*node)->shell, NULL, (*node)->shell->exit_status, 1);
		if ((*node && (*node)->done_lock == 1 && ft_back_node(*node)
				->lvl_subshell >= (*node)->shell->lvl_subshell
				&& (*node)->shell->lvl_subshell != 0))
			ft_clean_exit((*node)->shell, NULL, (*node)->shell->exit_status, 1);
		else
			return (1);
	}
	return (0);
}

void	norm_if2_exec(t_node **node)
{
	if (ft_get_op(*node) == PIPE
		|| (*node == go_to_starter_node((*node)->shell->tree->left))
		|| (norm_check(*node)))
		ft_single_cmd2(*node, ft_do_redir2);
	if ((*node)->shell->lvl_subshell > 0)
		ft_clean_exit((*node)->shell, NULL, (*node)->shell->exit_status, 1);
}

void	execute_recursivev2(t_node *node)
{
	t_node	*temp;
	t_node	*next_node;

	temp = node;
	while (1)
	{
		if ((node != NULL && next_cmd_same_lvl(node) && node->is_last == 1)
			|| (node && node->is_last == 1 && !next_cmd_same_lvl(node)
				&& node->done_lock == 0))
		{
			ft_dup2(&node->shell->new_temp_output, STDOUT_FILENO);
			norm_if2_exec(&node);
			return ;
		}
		if (norm_if_exec_recursive(&node, &temp))
			return ;
		if (ft_back_node(node)->lvl_subshell == node->shell->lvl_subshell
			|| node->is_last == 1 || node->is_last == 2)
			next_node = just_executev2(node);
		else if (ft_back_node(node)->lvl_subshell > node->shell->lvl_subshell
			&& node->is_last == 0)
			next_node = fork_executev2(node);
		node = next_node;
	}
}

void	fix_out_trunc(t_node *node)
{
	int	i;
	int	fd;

	i = -1;
	if (node->content.redir == NULL)
		return ;
	while (++i < node->content.kv_size)
	{
		if (node->content.redir[i].lvl > 0 && node->content.redir[i].key
			== R_OUTPUT_TRUNC)
		{
			fd = open(node->content.redir[i].value,
					O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0777);
			close(fd);
			node->content.redir[i].key = R_OUTPUT_APPEND;
		}
	}
}
