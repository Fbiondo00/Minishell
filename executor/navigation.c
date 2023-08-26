/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:46:46 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 00:54:19 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// dovrebbe essere simile a get_starter_node
// solo che prende un nodo come input
t_node	*go_to_starter_node(t_node *node)
{
	t_node	*nod;

	nod = node;
	while (nod)
	{
		if (!nod->left)
			break ;
		nod = nod->left;
	}
	return (nod);
}

int	is_left_branch(t_node *node)
{
	if (node == node->back->left)
		return (1);
	return (0);
}

int	is_node_cmd(t_node *node)
{
	if (node->content.idx_op == -1)
		return (1);
	return (0);
}

t_node	*next_cmd(t_shell *shell, t_node *node)
{
	if (node->back == shell->tree || (!is_left_branch(node)
			&& node->back->back == shell->tree))
	{
		return (NULL);
	}
	if (is_left_branch(node))
		return (node->back->right);
	else
	{
		printf("NEXT_CMD: IN ELSE \n");
		if (is_node_cmd(node->back->back->right))
			return (node->back->back->right);
		else if (node->back == node->back->back->right)
		{
			return (node->back->back->back->right);
		}
		else
		{
			go_to_starter_node(node->back->back->right)->back->lvl_subshell
				+= node->back->back->lvl_subshell;
			go_to_starter_node(node->back->back->right)->back->lvl_lock = 1;
			return (go_to_starter_node(node->back->back->right));
		}
	}
}

t_node	*next_cmd2(t_shell *shell, t_node *node)
{
	if (!is_left_branch(node) && node->back == shell->tree)
		return (NULL);
	if (is_left_branch(node))
	{
		if (is_node_cmd(node->back->right))
			return (node->back->right);
		else
		{
			return (go_to_starter_node(node->back->right));
		}
	}
	else
	{
		if (is_node_cmd(node->back->back->right))
			return (node->back->back->right);
		else
			return (go_to_starter_node(node->back->back->right));
	}
}
