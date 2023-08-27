/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   after_before.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 15:56:22 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:30:34 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// ritorna idx dell ultimo operatore
int	op_after(t_shell *shell, int idx)
{
	char	*chars;

	chars = "|&";
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ')' && !in_quotes_str(shell, idx))
			break ;
	}
	while (++idx < ft_strlen(shell->str))
	{
		if (ft_strchr(chars, shell->str[idx]) && !in_quotes_str(shell, idx))
		{
			if (ft_strchr(chars, shell->str[idx + 1]))
				return (idx + 1);
			else
				return (idx);
		}
	}
	return (0);
}

int	norm_char_after(t_shell *shell, int idx)
{
	if (idx == ft_strlen(shell->str))
		return (0);
	if (shell->str[idx] == ')' || shell->str[idx] == '|'
		|| shell->str[idx] == '&')
		return (0);
	return (1);
}

// dato idx controlla se successivaente e'presente un char
// anche redir va bene. il carattere ) o il fine linea
// non contano come char valido.
// deve anche skippare le redir.
int	char_after(t_shell *shell, int idx, int flag, char *op)
{
	while (++idx < ft_strlen(shell->str))
	{
		if (flag == 1)
		{
			if (ft_strchr(op, shell->str[idx]) && !in_quotes_str(shell, idx))
				idx = get_idx_after_redir(shell, idx, 1);
			if (shell->str[idx] != ' ' && !in_quotes_str(shell, idx))
				break ;
		}
		else
		{
			if (ft_strchr(op, shell->str[idx]) && !in_quotes_str(shell, idx))
			{
				idx = get_idx_after_redir(shell, idx, 0);
				if (idx == ft_strlen(shell->str) - 1)
					break ;
				else
					idx++;
			}
			if (shell->str[idx] != ' ' && !in_quotes_str(shell, idx))
				break ;
		}
	}
	return (norm_char_after(shell, idx));
}

// riceve idx di (
// verifica presenza di operatore fino a idx 0 o fino a (
// return  (idx) errore
// return 0 ok
int	op_before(t_shell *shell, int idx)
{
	char	*chars;

	chars = "|&";
	while (--idx >= 0)
	{
		if (shell->str[idx] == '(' && !in_quotes_str(shell, idx))
			break ;
		if (ft_strchr(chars, shell->str[idx]) && !in_quotes_str(shell, idx))
			return (idx);
	}
	return (0);
}

// ritorna idx se char before
// ritorna 0 in caso contrario
int	char_before(t_shell *shell, int idx)
{
	while (--idx >= 0)
	{
		if (shell->str[idx] != ' ')
			break ;
	}
	if (idx < 0)
		return (0);
	if (shell->str[idx] == '(')
		return (0);
	return (idx);
}

// 5 func