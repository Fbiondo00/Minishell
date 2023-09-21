/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 03:01:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/20 22:04:17 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_pointers(t_node **node, t_node **temp, t_node **next, t_node **res)
{
	*temp = *node;
	*next = next_cmd_same_lvl(*node);
	*res = NULL;
}

void	norm_if1(t_node **node, t_node **next)
{
	if ((*next)->done_lock == 1 || (*next && ft_back_node(*next)
			->lvl_subshell < (*next)->shell->lvl_subshell && (*next)->shell
			->lvl_subshell != 0))
		ft_clean_exit((*node)->shell, NULL, (*node)->shell->exit_status, 1);
}

void	norm_if2(t_node **node, t_node **resume)
{
	if ((*resume)->done_lock == 1 || (*resume && ft_back_node(*resume)
			->lvl_subshell < (*resume)->shell->lvl_subshell && (*resume)->shell
			->lvl_subshell != 0))
		ft_clean_exit((*node)->shell, NULL, (*node)->shell->exit_status, 1);
}

int	norm_if0(t_node **node, t_node **temp, t_node **next, t_node **res)
{
	init_pointers(node, temp, next, res);
	if (!norm_check(*node))
		if (next_cmd2((*node)->shell, *node) && ft_back_node(next_cmd2((*node)
					->shell, *node)))
			if (ft_back_node(next_cmd2((*node)->shell, *node))
				->content.op == PIPE)
				return (1);
	return (0);
}

t_node	*next_onev2(t_node *node)
{
	t_node	*next;
	t_node	*temp;
	t_node	*resume;

	if (norm_if0(&node, &temp, &next, &resume))
		return (go_next_cmd_and_or(node));
	if (norm_checkv2skip(node))
		return (next);
	if (!next)
		return (NULL);
	if (ft_back_node(next)->lvl_subshell > node->shell->lvl_subshell && node
		->is_last == 0)
		return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
	norm_if1(&node, &next);
	if (ft_get_op(next) == PIPE)
		return (next);
	if (ft_get_op(next) == AND || ft_get_op(next) == OR)
	{
		resume = next_cmd_same_lvl(next);
		if (!resume)
			return (NULL);
		norm_if2(&node, &resume);
		return (resume);
	}
	return (NULL);
}
