/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recursive.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:02:35 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/17 00:33:14 by rdolzi           ###   ########.fr       */
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

int	ft_char_ok(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9') || (c == 34 || c == 39))
		return (1);
	return (0);
}

int	check_is_redir(t_shell *shell, int idx)
{
	int	len;

	len = ft_strlen(shell->str);
	while (--len >= 0)
		if (shell->str[len] != ' ')
			break ;
	if ((!ft_char_ok(shell->str[len]) && !redir_after(shell, idx))
		|| (ft_char_ok(shell->str[len]) && redir_after(shell, idx)))
		return (1);
	return (0);
}

int	fix_lexer(t_shell *sh, int i)
{
	if (!op_after(sh, i) && ((!check_is_redir(sh, i)
				&& char_after(sh, get_end_parentheses(sh, i), 1, "><"))
			|| (!redir_after(sh, i) && char_after(sh, get_end_parentheses(sh, i)
					, 1, "><"))))
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
				|| fix_lexer(sh, i) || (empty_parentheses(sh, i)
					&& (flag == 1 || (par > 1 && left_p > 1))))
				return (1);
			if (par > 1 && par <= left_p)
				remove_par(sh, i);
		}
	}
	return (0);
}
