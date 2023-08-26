/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 22:02:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/25 18:51:19 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_node	*next_cmd_same_lvl(t_node *node)
{
	t_node	*temp;
	t_node	*next_node;

	temp = node;
	temp->done_lock = 1;
	next_node = NULL;
	while (1)
	{
		next_node = next_cmd2(node->shell, temp);
		if (temp == next_node)
			return (NULL);
		else
			temp = next_node;
		if (!next_node)
			break ;
		if (next_node && next_node->shell->lvl_subshell == node->lvl_subshell)
		{
			return (next_node);
		}
	}
	return (NULL);
}

t_node	*go_next_cmd_and_or(t_node *node)
{
	t_node	*next_node;

	next_node = NULL;
	while (1)
	{
		next_node = next_cmd_same_lvl(node);
		if (!next_node)
			break ;
		if (next_node->content.op != PIPE)
		{
			return (next_node);
		}
	}
	return (NULL);
}
