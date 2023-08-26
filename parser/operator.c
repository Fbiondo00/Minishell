/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operator.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 18:59:12 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/25 18:59:43 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	get_idx_next_op(t_node *node, int idx)
{
	char	*chars;

	chars = "|&";
	while (idx-- > 0)
	{
		if (ft_strchr(chars, node->raw_cmd[idx]) && !in_quotes(node, idx))
		{
			if (node->raw_cmd[idx - 1] == '&' || node->raw_cmd[idx - 1] == '|')
				return (idx - 1);
			else
				return (idx);
		}
	}
	return (0);
}

int	is_last_char(t_node *node, int idx)
{
	if (node->raw_cmd[idx + 1] == '\0')
		return (1);
	return (0);
}

int	get_op(t_node *node, int idx)
{
	if (node->raw_cmd[idx] == '&' && !in_quotes(node, idx))
	{
		if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '&')
			return (AND);
		else
			return (0);
	}
	if (node->raw_cmd[idx] == '|' && !in_quotes(node, idx))
	{
		if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '|')
			return (OR);
		else
			return (PIPE);
	}
	return (0);
}

int	there_is_logic_operator(t_node *node)
{
	int	next_op;
	int	idx;

	idx = ft_strlen(node->raw_cmd) - 1;
	while (--idx > 0)
	{
		next_op = get_idx_next_op(node, idx);
		if (get_op(node, next_op) == AND || get_op(node, next_op) == OR)
			return (next_op);
	}
	return (0);
}

int	check_op_logic_than_pipe(t_node *node)
{
	if (there_is_logic_operator(node))
	{
		printf("there_is_logic_operator\n");
		return (there_is_logic_operator(node));
	}
	if (get_idx_next_op(node, ft_strlen(node->raw_cmd) - 1))
	{
		printf("get_idx_next_op\n");
		return (get_idx_next_op(node, ft_strlen(node->raw_cmd) - 1));
	}
	return (0);
}
