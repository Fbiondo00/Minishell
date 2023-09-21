/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 02:59:39 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/20 22:03:47 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_node	*norm_return(t_node **node, t_node **next_node, t_node **prev_node)
{
	norm_node(node, prev_node, next_node);
	if (*node == NULL || (*node)->done_lock == 1)
	{
		if (*node)
			return (*node);
		else
			return (NULL);
	}
	return (*next_node);
}

void	norm_node(t_node **node, t_node **prev_node, t_node **next_node)
{
	(*node)->done_lock = 1;
	if (next_node != NULL)
		*prev_node = *node;
	*node = *next_node;
}

int	norm_check1(t_node *node, t_node *prev_node)
{
	return (ft_get_op(node) == PIPE || (!prev_node && node
			== go_to_starter_node(node->shell->tree->left))
		|| (prev_node && norm_check(node)) || (!prev_node
			&& node != go_to_starter_node(node->shell->tree->left)
			&& norm_check(node)));
}

void	norm_init_pointers(t_node **n, t_node **temp, t_node **pn, t_node **nn)
{
	*n = *temp;
	*pn = NULL;
	*nn = NULL;
	if ((*temp)->shell->lvl_subshell > 0)
		dup2((*temp)->shell->new_temp_output, STDOUT_FILENO);
}

t_node	*just_executev2(t_node *temp)
{
	t_node	*node;
	t_node	*next_node;
	t_node	*prev_node;

	norm_init_pointers(&node, &temp, &prev_node, &next_node);
	if (!node->content.cmd || !node->content.cmd[0])
	{
		if (norm_check1(node, prev_node))
		{
			if (ft_do_redir2(node) == 1)
				return (next_cmd_same_lvl(node));
			if (ft_do_redir2(node) == 2)
				return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
			next_node = next_cmd_same_lvl(node);
		}
		else
			return (next_cmd_same_lvl(node));
	}
	else if (ft_get_op(node) == PIPE)
		next_node = ft_do_pipe(node);
	else if (norm_check2(node))
		next_node = wrap_ft_single_cmd2(node);
	else if (ft_get_op(node) == AND || ft_get_op(node) == OR)
		next_node = ft_do_and_or(node, prev_node);
	return (norm_return(&node, &next_node, &prev_node));
}
