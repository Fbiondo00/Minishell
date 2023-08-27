/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recursive.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:02:35 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 16:34:57 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// controlla se l i-esimo char è all interno di quotes
// ritorna  1 in caso sia presente nelle double quotes "
// ritorna -1 in caso sia presente nelle single quotes '
// ritorna  0 in caso non sia in quotes
int	in_quotes_str(t_shell *shell, int index)
{
	int	i;
	int	d_quotes;
	int	s_quotes;

	i = -1;
	d_quotes = 0;
	s_quotes = 0;
	while (++i < index)
	{
		if (shell->quote_idx[i] == 34)
			d_quotes++;
		if (shell->quote_idx[i] == 39)
			s_quotes++;
	}
	if (s_quotes % 2 != 0)
		return (-1);
	if (d_quotes % 2 != 0)
		return (1);
	return (0);
}

int	recursive(t_shell *shell, int par, int left_p, int idx)
{
	static int	flag;

	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == '(' && !in_quotes_str(shell, idx))
		{
			par++;
			flag++;
			if (recursive(shell, par, left_p, idx))
				return (1);
			if ((op_before(shell, idx) < char_before(shell, idx))
				|| (op_before(shell, idx) && !char_before(shell,
						op_before(shell, idx))))
				return (1);
			if (!op_before(shell, idx) && char_before(shell, idx))
				return (1);
			if (op_after(shell, idx) && !char_after(shell, op_after(shell, idx),
					0, "><"))
				return (1);
			if (!op_after(shell, idx) && char_after(shell,
					get_end_parentheses(shell, idx), 1, "><"))
				return (1);
			if (empty_parentheses(shell, idx) && (flag == 1 || (par > 1
						&& left_p > 1)))
				return (1);
			if (par > 1 && par <= left_p)
				remove_par(shell, idx);
		}
	}
	return (0);
}

// 2 func