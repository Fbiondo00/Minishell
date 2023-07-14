/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operator.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 18:59:12 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/14 02:37:19 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// INPUT:
// 1. shell 2. index of start operator 3. direction to check
// in caso di && o || ritorna index del secondo char
// return index of first next operator if present.
// otherwise return 0
// N.B: ritorna sempre l index del primo char.
// non controlla se si tratta di && o & (|| o |), quindi quale op sia.
// ci pensa get_op() a determinare di che operatore si tratti.
int	get_idx_next_op(t_shell *shell, int idx, int dir)
{
	int		len;
	char	*chars;

	chars = "|&";
	len = ft_strlen(shell->rawline);
	if (dir == 1)
	{
		while (++idx < len)
		{
			if (ft_strchr(chars, shell->rawline[idx]) && !in_quotes(shell, idx))
				return (idx);
		}
		return (0);
	}
	else
	{
		while (--idx > 0)
		{
			if (ft_strchr(chars, shell->rawline[idx]) && !in_quotes(shell, idx))
			{
				if (shell->rawline[idx - 1] == '&'
					|| shell->rawline[idx - 1] == '|')
					return (idx - 1);
				else
					return (idx);
			}
		}
		return (0);
	}
}

// INPUT:
// 1. shell
// 2. index di start, contiene l operatore
// (N.B:può avere un altro index a seguire in caso di operatore logico)
// 3. direzione da controllare
// verifica la presenza dell operatore && ||, bypassando |
// in caso affermativo ritorna l index dell operatore
// in caso negativo ritorna 0
// low level calculation in_quotes()
int	there_is_logic_operator(t_shell *shell, int idx, int dir)
{
	//int	i;
	int	len;
	int	next_op;

	//i = -1;
	len = ft_strlen(shell->rawline);
	if (dir == 1)
	{
		if (get_op(shell, idx) == AND || get_op(shell, idx) == OR)
			idx++;
		while (++idx < len)
		{
			next_op = get_idx_next_op(shell, idx, dir);
			if (get_op(shell, next_op) == AND || get_op(shell, next_op) == OR)
				return (next_op);
		}
		return (0);
	}
	else
	{
		while (--idx > 0)
		{
			next_op = get_idx_next_op(shell, idx, dir);
			if (get_op(shell, next_op) == AND || get_op(shell, next_op) == OR)
				return (next_op); // o next_op -1 ??
		}
		return (0);
	}
}

// ritorna l index dell operatore nella direzione indicata se presente
// altrimenti torna -1
// l operatore logico ha la priorità sulla pipe
// perciò viene tornato per primo se presente
int	check_op_logic_than_pipe(t_shell *shell, int idx_start, int dir)
{
	if (there_is_logic_operator(shell, idx_start, dir))
	{
		printf("there_is_logic_operator\n");
		return (there_is_logic_operator(shell, idx_start, dir));
	}
	if (get_idx_next_op(shell, idx_start, dir))
	{
		printf("get_idx_next_op\n");
		return (get_idx_next_op(shell, idx_start, dir));
	}
	return (0);
}

// return 1 se è ultimo char
// return 0 se non è ultimo char
// evita il seg. fault
int	is_last_char(t_shell *shell, int idx)// da modificare, deve guardare generic str
{
	if (shell->rawline[idx + 1] == '\0')
		return (1);
	return (0);
}

// INPUT:
//  1. shell 2. index di start dell operatore
//  se è op_logico prende in input il primo index dei due char
//  COSA FA?
//  dato un index, ritorna il tipo di operatore tra: && || |
//  ritorna 0 se non è un operatore
int	get_op(t_shell *shell, int idx)
{
	if (shell->rawline[idx] == '&' && !in_quotes(shell, idx))
	{
		if (!is_last_char(shell, idx) && shell->rawline[idx + 1] == '&')
			return (AND);
		else
			return (0);
	}
	if (shell->rawline[idx] == '|' && !in_quotes(shell, idx))
	{
		if (!is_last_char(shell, idx) && shell->rawline[idx + 1] == '|')
			return (OR);
		else
			return (PIPE);
	}
	return (0);
}
