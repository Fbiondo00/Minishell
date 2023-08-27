/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:23:00 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 16:28:54 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_isalpha(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

// prende idx del primo char di redir
// restituisce idx dell ultimo char prima dello spazio
int	get_idx_after_redir(t_shell *shell, int idx, int flag)
{
	if (shell->str[idx + 1] == '<' || shell->str[idx + 1] == '>')
		idx++;
	while (shell->str[++idx] == ' ')
		;
	while (idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ' ')
			break ;
		else if (shell->str[idx] == 34)
		{
			while (shell->str[++idx] != 34)
				;
		}
		else if (shell->str[idx] == 39)
		{
			while (shell->str[++idx] != 39)
				;
		}
		if (shell->str[idx] != ' ')
			break ;
		idx++;
	}
	if (flag)
		return (idx + 1);
	return (idx);
}

void	norm_quotes(t_shell *shell, int q, int *quotes, int *idx)
{
	(*quotes)++;
	shell->quote_idx[*idx] = q;
	while (++(*idx) < ft_strlen(shell->rawline) && shell->rawline[*idx] != q)
		shell->quote_idx[*idx] = 48;
	if (shell->rawline[*idx] == q)
	{
		(*quotes)++;
		shell->quote_idx[*idx] = q;
	}
}

// ritorna 1 se i caratteri (" ') sono dispari
// ritorna 0 se i caratteri (" ') sono pari
// Crea stringa quote_idx. Ex:
// 0"0000"00000""0"000000"00000000''0
// "ciao'"  ok "" "'come'"   stai ''a
int	unclosed_quotes(t_shell *shell)
{
	int	i;
	int	s_quote;
	int	d_quote;

	i = -1;
	s_quote = 0;
	d_quote = 0;
	shell->quote_idx = malloc(ft_strlen(shell->rawline) + 1);
	while (++i < ft_strlen(shell->rawline))
	{
		shell->quote_idx[i] = 48;
		if (shell->rawline[i] == 34)
			norm_quotes(shell, 34, &d_quote, &i);
		else if (shell->rawline[i] == 39)
			norm_quotes(shell, 39, &s_quote, &i);
	}
	shell->quote_idx[i] = '\0';
	if (s_quote % 2 != 0 || d_quote % 2 != 0)
		return (1);
	return (0);
}

// 4 func