/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 02:59:39 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 15:12:00 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	norm_check1(t_node *node, t_node *prev_node)
{
	return (ft_get_op(node) == PIPE || (!prev_node && node
			== go_to_starter_node(node->shell->tree->left))
		|| (prev_node && norm_check(node)) || (!prev_node && node
			!= go_to_starter_node(node->shell->tree->left)
			&& norm_check(node)));
}

int	norm_check2(t_node *node)
{
	return (ft_back_node(node)->lvl_subshell
		> node->shell->lvl_subshell && node
		!= go_to_starter_node(node->shell->tree->left)
		&& ft_back_node(node)->content.op == PIPE);
}

t_node	*norm_if(t_node *node, t_node *prev_node, t_node *next_node)
{
	if (!node->content.cmd || !node->content.cmd[0])
	{
		if (norm_check1(node, prev_node))
		{
			if (ft_do_redir2(node) == 1)
				return (next_cmd_same_lvl(node));
			if (ft_do_redir2(node) == 2)
				return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
			next_node = next_cmd_same_lvl(node);
			norm_node(&node, &prev_node, next_node);
		}
		else
			return (next_cmd_same_lvl(node));
	}
	return (NULL);
}

t_node	*norm_return(t_node *node, t_node *next_node)
{
	if (!node || node->done_lock == 1)
	{
		if (node)
			return (node);
		else
			return (NULL);
	}
	return (next_node);
}

t_node	*just_executev2(t_node *temp)
{
	t_node	*res;
	t_node	*node;
	t_node	*next_node;
	t_node	*prev_node;

	node = temp;
	prev_node = NULL;
	next_node = NULL;
	while (1)
	{
		if (temp->shell->lvl_subshell > 0)
			dup2(temp->shell->new_temp_output, STDOUT_FILENO);
		res = norm_if(node, prev_node, next_node);
		if (res != NULL)
			return (res);
		else if (ft_get_op(node) == PIPE)
			next_node = ft_do_pipe(node);
		else if (norm_check2(node))
			next_node = wrap_ft_single_cmd2(node);
		else if (ft_get_op(node) == AND || ft_get_op(node) == OR)
			next_node = ft_do_and_or(node, prev_node);
		norm_node(&node, &prev_node, next_node);
		node = next_node;
		return (norm_return(node, next_node));
	}
}
