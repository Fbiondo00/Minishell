/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   component.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:49:26 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/03 02:43:17 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_lowercase_cmd(t_node *node)
{
	int	i;

	i = -1;
	while (node->content.cmd[0] && node->content.cmd[0][++i])
	{
		if (node->content.cmd[0][i] >= 65 && node->content.cmd[0][i] <= 90)
			node->content.cmd[0][i] += 32;
	}
}

// ritorna un int contenente la differenza tra parentesi "(" e quelle ")"
// sostituisce le parentesi con gli spazi
// fa anche il set_cmd
int	calculate_lvl_diff(t_node *node)
{
	int	i;
	int	count;

	i = -1;
	count = 0;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
		{
			node->raw_cmd[i] = ' ';
			count++;
		}
		if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
		{
			node->raw_cmd[i] = ' ';
			count--;
		}
	}
	set_cmd(node);
	return (count);
}

void	ft_ft(t_shell *s, t_node *n)
{
	int	l;

	while (1)
	{
		l = calculate_lvl_diff(n);
		if (is_left_branch(n))
		{
			n->back->lvl_lock = 1;
			n->back->lvl_subshell += l;
		}
		if (next_cmd2(s, n) && !is_left_branch(n) && !n->back->back->lvl_lock)
		{
			n->back->back->lvl_lock = 1;
			n->back->back->lvl_subshell += (l + n->back->lvl_subshell);
		}
		else if (next_cmd2(s, n) && !is_left_branch(n)
			&& n->back->back->lvl_lock)
		{
			n->back->back->lvl_lock = 1;
			n->back->back->back->lvl_subshell += (l + n->back->lvl_subshell);
		}
		print_node(s, n);
		n = next_cmd2(s, n);
		if (!n)
			break ;
	}
}

// V2 con next_cmd2
//  naviga l albero e:
//  se trova nodo_cmd setta exp e fa le varie rimozioni
// e setta lvl sui corretti nodi op
//  se trova nodo_op continua a navigare l albero
void	set_components(t_shell *shell)
{
	t_node	*node;

	printf("------------------|FASE: SET COMPONENTS ALBERO|------------------\n");
	if (is_node_cmd(shell->tree))
	{
		calculate_lvl_diff(shell->tree);
		print_node(shell, shell->tree);
		return ;
	}
	node = go_to_starter_node(shell->tree->left);
	ft_ft(shell, node);
}
