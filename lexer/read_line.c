/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:40:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/23 00:30:29 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// controlla se l i-esimo char è all interno di quotes
// ritorna  1 in caso sia presente nelle double quotes "
// ritorna -1 in caso sia presente nelle single quotes '
// ritorna  0 in caso non sia in quotes
int in_quotes_str(t_shell *shell, int index)
{
	int i;
	int d_quotes;
	int s_quotes;

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

// ritorna 1:
//  - se i caratteri (  ) sono dispari o se sono nell ordine sbagliato )(
// ritorna il numero di occorrenza "(":
//  - se i caratteri (  ) sono pari e nell ordine giusto
// 41 == )
// 40 == (
int 	unclosed_parentheses(t_shell *shell)
{
	int	i;
	int	left_p;
	int	right_p;

	i = -1;
	left_p = 0;
	right_p = 0;
	while (++i < ft_strlen(shell->str))
	{
		if (shell->str[i] == 41 && !in_quotes_str(shell, i))
		{
			right_p++;
			if (right_p > left_p)
				return (-1);	
		}
		else if (shell->str[i] == 40 && !in_quotes_str(shell, i))
			left_p++;
	}
	if (left_p != right_p)
		return (-1);
	return (left_p);
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
		{
			d_quote++;
			shell->quote_idx[i] = 34;
			while (++i < ft_strlen(shell->rawline) && shell->rawline[i] != 34)
				shell->quote_idx[i] = 48;
			if (shell->rawline[i] == 34)
			{
				d_quote++;
				shell->quote_idx[i] = 34;
			}
		}
		else if (shell->rawline[i] == 39)
		{
			s_quote++;
			shell->quote_idx[i] = 39;
			while (++i < ft_strlen(shell->rawline) && shell->rawline[i] != 39)
				shell->quote_idx[i] = 48;
			if (shell->rawline[i] == 39)
			{
				s_quote++;
				shell->quote_idx[i] = 39;
			}
		}
	}
	shell->quote_idx[i] = '\0';
	if (s_quote % 2 != 0 || d_quote % 2 != 0)
		return (1);
	return (0);
}

// ritorna 1 se empty, 0 in caso contrario
// idx coincide con la parentesi di apertura
// 41 == )
int empty_parentheses(t_shell *shell, int idx)
{
	int len;

	len = ft_strlen(shell->str);
	while (++idx < len)
	{
		if (shell->str[idx] == 41 && !in_quotes_str(shell, idx))
			break;
		if (shell->str[idx] != ' ')
			return (0);
	}
	return (1);
}

// riceve idx di (
// verifica presenza di operatore fino a idx 0 o fino a (
// return  (idx) errore
// return 0 ok
int	op_before(t_shell *shell, int idx)
{
	char *chars;

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

// ritorna idx dell ultimo operatore
int op_after(t_shell *shell, int idx)
{
	char *chars;

	chars = "|&";
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ')' && !in_quotes_str(shell, idx))
			break;
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

//-------- NB  --------
// essendo ricorsiva lavoreremo sempre con ultima (
// modifica da op o ( precedente a sinistra e ) a destra
void remove_par(t_shell *shell, int idx)
{
	int i;
	int min;
	int max;
	char *chars;

	chars = "(|&";
	min = idx;
	while (--min >= 0)
	{
		if (ft_strchr(chars, shell->str[min]) && !in_quotes_str(shell, min))
			break ;
	}
	max = idx;
	while (++max < ft_strlen( shell->str))
	{
		if (shell->str[max] == ')' && !in_quotes_str(shell, max))
			break ;
	}
	i = -1;
	while( ++i < ft_strlen(shell->str))
	{
		if (i >= min && i <= max)
		{
			if (shell->str[i] == '(' || shell->str[i] == ')')
				shell->str[i] = 'a';
		}
	}
}

// ritorna idx se char before
// ritorna 0 in caso contrario
int char_before(t_shell *shell, int idx)
{
	while (--idx >= 0)
	{
		if (shell->str[idx] != ' ')
			break ;
	}
	if (idx < 0 )
		return (0);
	if (shell->str[idx] == '(')
		return (0);
	return (idx);
}

// prende idx del primo char di redir
// restituisce idx dell ultimo char prima dello spazio
int get_idx_after_redir(t_shell *shell, int idx)
{
	if (shell->str[idx + 1] == '<' || shell->str[idx + 1] == '>')
		idx++;
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ' ')
			break;
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
	}
	return (idx  - 1);
}

// dato idx controlla se successivaente e'presente un char
// anche redir va bene. il carattere ) o il fine linea 
// non contano come char valido.
// deve anche skippare le redir.
int char_after(t_shell *shell, int idx, int flag)
{
	char *op;

	op = "><";
	while (++idx < ft_strlen(shell->str))
	{
		if (flag)
		{
			if (shell->str[idx] != ' ' && !ft_strchr(op, shell->str[idx]))
				break;
			if (shell->str[idx] != ' ' && ft_strchr(op, shell->str[idx]))
				idx = get_idx_after_redir(shell, idx);
		}
		else
		{
			if (shell->str[idx] != ' ')
				break;
		}
	}
	if (idx == ft_strlen(shell->str))
		return (0);
	if (shell->str[idx] == ')')
		return (0);
	return (1);
}

// riceve idx di ( restituisce idx di )
int get_end_parentheses(t_shell *shell, int idx)
{
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ')' && !in_quotes_str(shell, idx))
			return (idx);
	}
	return (0);
}

int recursive(t_shell *shell, int par, int left_p, int idx)
{
	static int flag;

	printf("recursive(shell, par:%d, left_p:%d, idx:%d, flag:%d)\n", par, left_p, idx, flag);
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == '(' && !in_quotes_str(shell, idx))
		{
			par++;
			flag++;
			if (recursive(shell, par, left_p, idx))
				return (1);
			printf("op_before(sh, idx:%d):%d\n", idx, op_before(shell, idx));
			printf("char_before(shell, op_before(shell, idx)):%d\n", char_before(shell, op_before(shell, idx)));
			if (op_before(shell, idx) < char_before(shell, idx))
			{
				printf("error:op_before(shell, idx) < char_before(shell, idx)\n");
				printf("SBALLA TUTTO??\n");
				return (1);
			}
			if ((op_before(shell, idx) && !char_before(shell, op_before(shell, idx))) )
			{
				printf("error, is present op_before\n");
				return (1);
			}
			if (!op_before(shell, idx) && char_before(shell, idx))
			{
				printf("error missing op_before\n");
				return (1);
			}
			if ((op_after(shell, idx) && !char_after(shell, op_after(shell, idx), 1)))
			{
				printf("error, is present op_after\n");
				return (1);
			}
			if (!op_after(shell, idx) && char_after(shell, get_end_parentheses(shell, idx), 1))
			{
				printf("error missing op_after\n");
				return (1);
			}
			printf("empty_parentheses(shell, idx:%d):%d\n", idx, empty_parentheses(shell, idx));
			printf("flag:%d, par:%d\n", flag, par);
			if (empty_parentheses(shell, idx) && (flag == 1 || (par > 1 && left_p > 1)))
			{
				printf("error, empty_parentheses\n");
				return (1);
			}
			if (par > 1 && par <= left_p)
			{
				remove_par(shell, idx);
				printf("REMOVED:%s\n", shell->str);
			}
		}
	}
	return (0);
}

char *duplica(char *str)
{
	int i;
	char *new;

	i = -1;
	new = malloc(ft_strlen(str));
	while (++i < ft_strlen(str))
		new[i] = str[i];
	new[ft_strlen(str)] = '\0';
	return (new);
}

// 1 errore 0 ok
int check_operators(t_shell *shell)
{
	int i;
	char *chars;
	
	i = -1;
	chars = "|&";
	shell->str = duplica(shell->rawline);

	if (ft_strchr(chars, shell->str[ft_strlen(shell->str)- 1]))
		return (1);
	while (++i < ft_strlen(shell->str))
	{
		if (ft_strchr(chars, shell->str[i]) && !in_quotes_str(shell, i))
		{
			if (!char_before(shell, i))
			{
				printf("ERRORE:check_operators:!char_before\n");
				return (1);
			}
			if (ft_strchr(chars, shell->str[i + 1]))
				i++;
			if (!char_after(shell, i, 0))
			{
				printf("ERRORE:check_operators:!char_after\n");
				return (1);
			}
		}
	}
	return (0);
}

int check_parentheses(t_shell *shell)
{
	int left_p;

	left_p = unclosed_parentheses(shell);
	if (left_p == 0)
		return (0);
	if (left_p == -1)
	{
		printf("Esce in unclosed_parentheses!\n");
		return (1);
	}
	printf("left_p:%d\n", left_p);
	printf("start recursive...\n");
	if (recursive(shell, 0, left_p, -1))
	{
		printf("Esce in recursive check!\n");
		return (1);
	}
	return (0);
}

void	ft_read_line(t_shell *shell)
{
	shell->rawline = readline(PROMPT_MSG);
	if (!shell->rawline)
		exit(101);
	if (shell->rawline && shell->rawline[0])
	{
		add_history(shell->rawline);
		printf("------------------|FASE: CHECK SINTASSI|------------------\n");
		if (unclosed_quotes(shell))
		{
			ft_clean_exit(shell, UNCLOSED_QUOTES_ERROR, 1, 0);
			return;
		}
		if (check_operators(shell) || check_parentheses(shell))
		{
			ft_clean_exit(shell, SYNTAX_ERROR, 1, 0);
			return;
		}
	}
}

// 16 func