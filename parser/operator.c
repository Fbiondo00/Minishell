/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operator.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 18:59:12 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 14:57:27 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// controlla il next_op a partire da idx
// va sempre verso sinistra
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

// return 1 se è ultimo char
// return 0 se non è ultimo char
// evita il seg. fault
int	is_last_char(t_node *node, int idx)
{
	if (node->raw_cmd[idx + 1] == '\0')
		return (1);
	return (0);
}

//  dato un index, ritorna il tipo di operatore tra: && || |
//  ritorna 0 se non è un operatore
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

// verifica la presenza dell operatore && ||, bypassando |
// in caso affermativo ritorna l index dell operatore
// in caso negativo ritorna 0
// N.B: parte sempre dall ultimo index e va sempre verso sinistra
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

// COSA FA?
// ritorna l index dell operatore nella direzione indicata se presente
// altrimenti torna -1
// l operatore logico ha la priorità sulla pipe
// perciò viene tornato per primo se presente
// prende la raw_cmd e controlla sempre da destra verso sinistra
// anche there_is_logic_operator e get_idx_next_op prendono il primo
// a partire da destra e quindi startano dall ultimo index
int	check_op_logic_than_pipe(t_node *node)
{
	if (there_is_logic_operator(node))
	{
		return (there_is_logic_operator(node));
	}
	if (get_idx_next_op(node, ft_strlen(node->raw_cmd) - 1))
	{
		return (get_idx_next_op(node, ft_strlen(node->raw_cmd) - 1));
	}
	return (0);
}

// 5 func