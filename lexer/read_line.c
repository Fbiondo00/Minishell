/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:40:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/26 19:36:36 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


// 1 presente 0 assente
int redir_after(t_shell *shell, int idx)
{
	char *op;
	char *chars;

	op = "><";
	chars = "|&";
	while (++idx < ft_strlen(shell->str))
	{
		if (ft_strchr(chars, shell->str[idx]) && !in_quotes_str(shell, idx))
			return (0);

		if (ft_strchr(op, shell->str[idx]) && !in_quotes_str(shell, idx))
			return (1);
	}
	return (0);
}

// 1 errore 0 ok
int check_operators(t_shell *shell)
{
	int i;
	char *chars;
	
	i = -1;
	chars = "|&";
	if (ft_strchr(chars, shell->str[ft_strlen(shell->str)- 1]))
		return (1);
	while (++i < ft_strlen(shell->str))
	{
		if (ft_strchr(chars, shell->str[i]) && !in_quotes_str(shell, i))
		{
			if (!char_before(shell, i) && (!ft_isalpha(shell->str[0]) && !ft_isdigit2(shell->str[0])))
				return (1);
			if (ft_strchr(chars, shell->str[i + 1]))
				i++;
			if (!char_after(shell, i, 0, "><") && !redir_after(shell, i))
				return (1);
		}
	}
	return (0);
}

// ritorna 1 errore, ritorna 0 ok
// idx: indice del primo char dopo | o &
int check_other_stuff(t_shell *shell, int idx, int count)
{
	int i;

	i = -1;
	if (shell->str[idx - 1] == '&')
	{
		if (count == 1 && (shell->str[idx] == '>' || shell->str[idx] == '<'))
			return (1);
	}
	return (0);
}

// ritorna 1 errore, ritorna 0 ok
// ok: è il char dell op da controllare
// ko: è il set di chars che non devono essere presenti
int check_op(t_shell *shell, char ok, char *ko)
{
	int i;
	int count;

	i = -1;
	count = 0;
	while (++i < ft_strlen(shell->str))
	{
		if (shell->str[i] == ok && !in_quotes_str(shell, i))
		{
			count++;
			while (++i < ft_strlen(shell->str))
			{
				if (shell->str[i] == ok)
					count++;
				else if (ft_strchr(ko, shell->str[i]))
					return (1);
				else
					return (check_other_stuff(shell, i, count));
			}
			if (count > 2)
				return (1);
		}
	}
	return (check_other_stuff(shell, i, count));
}


void ft_read_line(t_shell *shell)
{
	shell->rawline = readline(PROMPT_MSG);
	if (!shell->rawline)
		exit(101);
	if (shell->rawline && shell->rawline[0])
	{
		add_history(shell->rawline);
		if (unclosed_quotes(shell))
		{
			ft_clean_exit(shell, UNCLOSED_QUOTES_ERROR, 1, 0);
			return;
		}
		shell->str = duplica(shell->rawline);
		if (check_op(shell, '|', "&") || check_op(shell, '&', "|") || check_op(shell, '>', "&<|)(") || check_op(shell, '<', "&>|)(") || empty_redir(shell) || check_operators(shell) || check_parentheses(shell))
		{
			ft_clean_exit(shell, SYNTAX_ERROR, 1, 0);
			return;
		}
	}
}

// 5 func
