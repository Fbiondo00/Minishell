/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:46:46 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/17 22:12:40 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

// al momento credo vada utilizzato solo sui nodi_cmd e non op
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

t_node	*next_cmd1(t_shell *shell, t_node *node, t_node *to_check)
{
	if (node->back->back)
	{
		if ((!node->back->back->back && node->back == node->shell->tree->right)
			|| !node->back->back->right)
			return (NULL);
		if (node->back->back->right == node->back)
			to_check = node->back->back->back->right;
		else
			to_check = node->back->back->right;
		if (is_node_cmd(to_check))
			return (to_check);
		else
			return (go_to_starter_node(to_check));
	}
	else
		return (NULL);
}

// torna il generico next_cmd
// entra sempre un node_cmd, esce sempre un node_cmd (o NULL)
t_node	*next_cmd2(t_shell *shell, t_node *node)
{
	t_node	*to_check;
	t_node	*tmp;

	to_check = NULL;
	if (!node || shell->tree == node)
		return (NULL);
	if (is_left_branch(node))
	{
		if (is_node_cmd(node->back->right))
			return (node->back->right);
		else
			return (go_to_starter_node(node->back->right));
	}
	else
	{
		tmp = next_cmd1(shell, node, to_check);
	}
}
// 5 func