/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:40:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/30 19:02:37 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//1.  LEXER 
	//A. manda il prompt quando aspetta per un nuovo comando
		// viene interrotto dai segnali CTRL+D CTRL+C  (CTRL+\ non deve mandare alcun segnale?)
			// intercettare segnale con signal (no thread)
	//B. una volta ricevuto la linea di comando (da cosa viene terminata?)
		// controlla se errore delle " o ' non chiuse
// C. controllo syntax
	//  SYNTAX ERROR:
		// 1. primo/ultimo char è un operatore
		// 2. dopo () deve essere presente  redirection o operatore
			//NON puo essere vuoto ma deve essere presente un char diverso da spazio
		// 3. unclosed quotes

// ----------------------------------------------------------------------

// controlla se l i-esimo char è all interno di quotes (sia " che ')
// ritorna 1 in caso sia presente nelle double quotes "
// ritorna -1 in caso sia presente nelle single quotes '
// 0 in caso non sia all interno di nessuna delle due
// 0"0000"00000""0"000000"00000000''0
// "ciao'"  ok "" "'come'"   stai ''a
int in_quotes_str(char *str, int index)
{
    int i;
    int d_quotes;
    int s_quotes;

    i = -1;
    d_quotes = 0;
    s_quotes = 0;
    while (++i < index)
    {
        if (str[i] == 34)
            d_quotes++;
        if (str[i] == 39)
            s_quotes++;
    }
    if (s_quotes % 2 != 0)
        return (-1);
    if (d_quotes % 2 != 0)
        return (1);
    return (0);
}

// ritorna 1 se i caratteri (  ) sono dispari o se sono nell ordine sbagliato )(
// ritorna 0 se i caratteri (  ) sono pari e nell ordine giusto
// ritorna il numero di left parentesi se tutto ok
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
		if (shell->str[i] == 41 && !in_quotes_str(shell->str, i)) // 41 == )
		{
			right_p++;
			if (right_p > left_p)
				return (-1);	
		}

		else if (shell->str[i] == 40 && !in_quotes_str(shell->str, i)) // 40 == (
			left_p++;
	}
	if (left_p != right_p)
		return (-1);
	return (left_p);
}

// // ritorna 1 se il primo o ultimo elemento (esclusi gli spazi) è un operatore
// V2 per ( |pwd) -> BUG
int	check_operators(t_shell *shell)
{
	int	i;
	int len;
	char *chars;

	i = -1;
	chars = "|&";
	len = ft_strlen(shell->rawline);
	while (++i < len)
	{
		if (shell->rawline[i] != ' ')
			break ;
	}
	if (ft_strchr(chars, shell->rawline[i]))
		return (1);
	while (--len >= 0)
	{
		if (shell->rawline[len] != ' ')
			break;
	}
	if (ft_strchr(chars, shell->rawline[len]))
		return (1);
	return (0);
}

// ritorna 1 se i caratteri (" ') sono dispari
// ritorna 0 se i caratteri (" ') sono pari
// Esempio: stringa echo 'ciao"' & echo "ciao'" è valida.
// ovvero non sono da considerare se inseriti in una coppia già chiusa
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

//V2 con idx (che coincide con la parentesi di apertura)
// ritorna 1 se empty, 0 in caso contrario
int empty_parentheses(t_shell *shell, int idx)
{
	int len;
	int count;

	count = 0;
	len = ft_strlen(shell->str);
	while (++idx < len)
	{
		if (shell->str[idx] == 41 && !in_quotes_str(shell->str, idx)) // 41 == )
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
		if (shell->str[idx] == '(' && !in_quotes_str(shell->str, idx))
			break ;
		if (ft_strchr(chars, shell->str[idx]) && !in_quotes_str(shell->str, idx))
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
		if (shell->str[idx] == ')' && !in_quotes_str(shell->str, idx))
			break;
	}
	while (++idx < ft_strlen(shell->str))
	{
		if (ft_strchr(chars, shell->str[idx]) && !in_quotes_str(shell->str, idx))
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
// quindi da eliminare e'fino a ) o fino a fine stringa
// elimina da op o ( precedente a sinistra e ) a destra
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
		if (ft_strchr(chars, shell->str[min]) && !in_quotes_str(shell->str, min))
			break ;
	}
	max = idx;
	while (++max >= ft_strlen( shell->str))
	{
		if (shell->str[max] == ')' && !in_quotes_str(shell->str, max))
			break ;
	}
	i = 0;
	while( i++ < ft_strlen(shell->str))
	{
		if (i >= min && i <= max)
			shell->str[i] = ' ';
	}
}

// ritorna se char before
int char_before(t_shell *shell, int idx)
{
	while (--idx >= 0)
	{
		if (shell->str[idx] != ' ')
			break ;
	}
	if (idx < 0 )
	{
		printf("no_char_before:index<0\n");
		return (0);
	}
	if (shell->str[idx] == '(')
	{
		printf("no_char_before: reached(\n");
		return (0);
	}
	return (1);
}

// dato idx, controlla se successivaente e'presente un char, anche redir va bene
// il carattere ) o il fine linea non conta come valido
int char_after(t_shell *shell, int idx)
{
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] != ' ')
			break ;
	}
	if (idx == ft_strlen(shell->str))
	{
		printf("char after:end reached\n");
		return (0);
	}
	if (shell->str[idx] == ')')
	{
		printf("char after: found )\n");
		return (0);
	}
	return (1);
}

// riceve idx of (, restituisce idx of his )
int get_end_parentheses(t_shell *shell, int idx)
{
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == ')' && !in_quotes_str(shell->str, idx))
			return (idx);
	}
	return (0);
}

int recursive(t_shell *shell, int par, int left_p, int idx)
{
	while (++idx < ft_strlen(shell->str))
	{
		if (shell->str[idx] == '(' && !in_quotes_str(shell->str, idx))
		{
			par++;
			if (recursive(shell, par, left_p, idx))
				return (1);
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
			if ((op_after(shell, idx) && !char_after(shell, op_after(shell, idx))))
			{
				printf("error, is present op_after\n");
				return (1);
			}
			if (!op_after(shell, idx) && char_after(shell, get_end_parentheses(shell, idx)))
			{
				printf("error missing op_after\n");
				return (1);
			}
			if (empty_parentheses(shell, idx))
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

// V2
// in generale:
// (()): errore
// ((ls)):ls ok
// if case_base(shell)
// return without error
int check_parentheses(t_shell *shell)
{
	int left_p;

	shell->str = duplica(shell->rawline);
	left_p = unclosed_parentheses(shell);
	if (left_p == 0)
		return (0);
	if (left_p == -1)
	{
		printf("Esce in unclosed_parentheses!\n");
		return (1);
	}
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
		if (unclosed_quotes(shell))
			ft_clean_exit(shell, UNCLOSED_QUOTES_ERROR, 2, 1);
		if (check_operators(shell) || check_parentheses(shell))
			ft_clean_exit(shell, SYNTAX_ERROR, 3, 1);
	}
}


// ------------------- WIP -------------------
// casi rimanenti parentesi:
// syntax nested parentheses:
// 1. se ((ls)) o (())  ((ee)) --> fare niente, non controllare nemmeno se non esiste comando
// ovvero non syntax error ma ignora
// 1.a(((ls)))   di fatto dovrebbe essere pure dinamico... questo rientra nel caso 
// 1.b ((ls) ), secondo questa regola questo viene eseguito. e di fatto e'corretto!
// non rientra nel caso precedente.
// viene controllato all inizio, il caso non arriva alla funzione ricorsiva
// il check potrebbe essere se sono 2 ( e se i primi due idx sono (( e ultimi due sono ))
// 2. BUG: ((ls) | (ls))
// 3. BUG: ( |pwd)  --> lo stesso check per le ( ) va fatto anche se non sono presenti

