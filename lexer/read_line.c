/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:40:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/01 17:11:34 by flaviobiond      ###   ########.fr       */
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

// upgrade per check parentesi
// int in_quotes_str(char *str, int index)
// {
//     int i;
//     int d_quotes;
//     int s_quotes;

//     i = -1;
//     d_quotes = 0;
//     s_quotes = 0;
//     while (++i < index)
//     {
//         if (str[i] == 34)
//             d_quotes++;
//         if (str[i] == 39)
//             s_quotes++;
//     }
//     if (s_quotes % 2 != 0)
//         return (-1);
//     if (d_quotes % 2 != 0)
//         return (1);
//     return (0);
// }

// // ritorna 1 se i caratteri (  ) sono dispari
// // ritorna 0 se i caratteri (  ) sono pari
// int 	unclosed_parentheses(t_shell *shell)
// {
// 	int	i;
// 	int	left_p;
// 	int	right_p;

// 	i = -1;
// 	left_p = 0;
// 	right_p = 0;
// 	while (++i < ft_strlen(shell->rawline))
// 	{
// 		if (shell->rawline[i] == 41 && !in_quotes_str(shell->rawline, i)) // 41 == )
// 			right_p++;
// 		else if (shell->rawline[i] == 40 && !in_quotes_str(shell->rawline, i)) // 40 == (
// 			left_p++;
// 	}
// 	if (left_p % 2 != 0 || right_p % 2 != 0)
// 		return (1);
// 	return (0);
// }

// // dopo () valide deve essere presente  redirection o operatore
// //NON puo essere vuoto ma deve essere presente un char diverso da spazio
// // ritorna 1 in caso di errore sintassi, 0 in caso contrario
// int check_parentheses(t_shell *shell)
// {
// 	if (unclosed_parentheses(shell))
// 		return (1);
// 	if (empty_parentheses(shell))
// 		return (1);
// 	// if (not_operator_in_multiple_cmd(shell))
// 	// 	return (1);
// 	return (0);
// }

// // ritorna 1 se il primo o ultimo elemento (esclusi gli spazi) è un operatore
// int	check_operators(t_shell *shell)
// {
// 	int	i;
// 	int len;
// 	char *chars;

// 	i = -1;
// 	chars = "|&";
// 	len = ft_strlen(shell->rawline);
// 	while (++i < len)
// 	{
// 		if (shell->rawline[i] != ' ')
// 			break ;
// 	}
// 	printf("shell->rawline[i]:%c\n", shell->rawline[i]);
// 	if (ft_strchr(chars, shell->rawline[i]))
// 		return (1);
// 	while (--len >= 0)
// 	{
// 		if (shell->rawline[len] != ' ')
// 			break;
// 	}
// 	printf("shell->rawline[len]:%c\n", shell->rawline[len]);
// 	if (ft_strchr(chars, shell->rawline[len]))
// 		return (1);
// 	return (0);
// }

// ritorna 1 se i caratteri (" ') sono dispari
// ritorna 0 se i caratteri (" ') sono pari
// setta un array di char, di lunghezza della line che negli index esatti
// conserva o " o ', ripuliti dalle quotes da non considera 
// Esempio: stringa echo 'ciao"' & echo "ciao'" è valida.
// ovvero non sono da considerare se inseriti in una coppia già chiusa
// il counter double o single si ferma in caso sia iniziato
//              ma non concluso l altro



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

void	ft_read_line(t_shell *shell)
{
	shell->rawline = readline(PROMPT_MSG);

	 // is to free?
	if (!shell->rawline)
		exit(101); // leaks readline?
	if (shell->rawline && ft_strlen(shell->rawline) !=0)
		add_history(shell->rawline);
	// --- syntax checks ---
	// a questo punto malloc quote_idx e forse rawline
	if (unclosed_quotes(shell)) // check_quote se è da fare void con exit
		ft_clean_exit(shell, UNCLOSED_QUOTES_ERROR, 2);
	// if (check_operators(shell) || check_parentheses(shell))
	// 	ft_clean_exit(shell, UNCLOSED_QUOTES_ERROR, 3);
	// if (check_operators(shell))
	// 	ft_clean_exit(shell, SYNTAX_ERROR, 3);
	// if (check_parentheses(shell))
	// 	ft_clean_exit(shell, SYNTAX_ERROR, 3);
}

