/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 02:02:56 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 02:09:57 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// dato un nodo ritorna l op corrispettivo
// se è left -> back
// se è right -> back -> back
// se è last -> back
// non dovrebbe mai tornare 0
int	ft_get_op(t_node *node)
{
	if (!next_cmd2(node->shell, node))
		return (node->back->content.op);
	if (is_left_branch(node))
		return (node->back->content.op);
	if (!is_left_branch(node))
		return (node->back->back->content.op);
	return (0);
}

t_node	*ft_back_node(t_node *node)
{
	if (!next_cmd2(node->shell, node))
		return (node->back);
	if (is_left_branch(node))
		return (node->back);
	if (!is_left_branch(node))
	{
		if (node->back->back->right == node->back)
		{
			if (node->back->back->back)
				return (node->back->back->back);
			else
				return (node->back);
		}
		return (node->back->back);
	}
	return (0);
}

// se presente un node_op con lvl subshell < 0, lo setta a 0
void	fix_redir(t_node *node)
{
	t_node	*temp;
	t_node	*next_node;

	temp = node;
	next_node = NULL;
	while (1)
	{
		fix_out_trunc(temp);
		next_node = next_cmd2(node->shell, temp);
		if (!next_node)
			return ;
		temp = next_node;
	}
}

// se presente un node_op con lvl subshell < 0, lo setta a 0
void	fix_lvl_subshell(t_node *node)
{
	t_node	*temp;
	t_node	*next_node;

	temp = node;
	next_node = NULL;
	while (1)
	{
		if (temp->back->lvl_subshell < 0)
			temp->back->lvl_subshell = 0;
		next_node = next_cmd2(node->shell, temp);
		if (!next_node)
			return ;
		temp = next_node;
	}
}

void	executev2(t_shell *shell)
{
	if (is_node_cmd(shell->tree))
		execute_single_cmd(shell->tree);
	else
	{
		fix_redir(go_to_starter_node(shell->tree->left));
		fix_lvl_subshell(go_to_starter_node(shell->tree->left));
		ft_do_heredoc(go_to_starter_node(shell->tree->left));
		execute_recursivev2(go_to_starter_node(shell->tree->left));
	}
}
