/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recursive.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:02:35 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:30:56 by rdolzi           ###   ########.fr       */
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

int	recursive(t_shell *sh, int par, int left_p, int i)
{
	static int	flag;

	while (++i < ft_strlen(sh->str))
	{
		if (sh->str[i] == '(' && !in_quotes_str(sh, i))
		{
			par++;
			flag++;
			if (recursive(sh, par, left_p, i))
				return (1);
			if ((op_before(sh, i) < char_before(sh, i)) || (op_before(sh, i)
					&& !char_before(sh, op_before(sh, i))))
				return (1);
			if ((!op_before(sh, i) && char_before(sh, i)) || (op_after(sh, i)
					&& !char_after(sh, op_after(sh, i), 0, "><"))
				|| (!op_after(sh, i) && char_after(sh, get_end_parentheses(sh,
							i), 1, "><")) || (empty_parentheses(sh, i)
					&& (flag == 1 || (par > 1 && left_p > 1))))
				return (1);
			if (par > 1 && par <= left_p)
				remove_par(sh, i);
		}
	}
	return (0);
}
