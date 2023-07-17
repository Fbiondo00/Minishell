/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:40:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/15 14:32:05 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//1.  LEXER 
	//A. manda il prompt quando aspetta per un nuovo comando
		// viene interrotto dai segnali CTRL+D CTRL+C  (CTRL+\ non deve mandare alcun segnale?)
			// intercettare segnale con signal (no thread)
	//B. una volta ricevuto la linea di comando (da cosa viene terminata?)
		// controlla se errore delle " o ' non chiuse
	//C. Se viene intercettata la UP-ARROW entra in "history mode".
	//   in tale modalità possono essere utilizzate: 
	// UP-ARROW DOWN-ARROW  per navigazione
// ----------------------------------------------------------------------

// ritorna 1 se i caratteri (" ') sono dispari
// ritorna 0 se i caratteri (" ') sono pari
// setta un array di char, di lunghezza della line che negli index esatti
// conserva o " o ', ripuliti dalle quotes da non considera 
// Esempio: stringa echo 'ciao"' & echo "ciao'" è valida.
// ovvero non sono da considerare se inseriti in una coppia già chiusa
// il counter double o single si ferma in caso sia iniziato
//              ma non concluso l altro
// TBD è ancora il caso?: essendo len della line un dato dinamico (expansions) il dato deve essere ricalcolato ogni volta?
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

// "00000"00000""0"000000"00000000''
// "ciao'"  ok "" "'come'"   stai ''
void	print_str(char *str)
{
	int	i;

	i = -1;
	while (++i < ft_strlen(str))
		printf("%c", str[i]);
	printf("\n");
}

// controlla se l i-esimo char è all interno di quotes (sia " che ')
// ritorna 1 in caso sia presente nelle double quotes "
// ritorna -1 in caso sia presente nelle single quotes '
// 0 in caso non sia all interno di nessuna delle due
// 0"0000"00000""0"000000"00000000''0
// "ciao'"  ok "" "'come'"   stai ''a
// TODO:test char operator / redirections in quotes
// int	in_quotes(t_shell *shell, int index)
// {
// 	int	i;
// 	int	d_quotes;
// 	int	s_quotes;

// 	i = -1;
// 	d_quotes = 0;
// 	s_quotes = 0;
// 	while (++i < index)
// 	{
// 		if (shell->quote_idx[i] == 34)
// 			d_quotes++;
// 		if (shell->quote_idx[i] == 39)
// 			s_quotes++;
// 	}
// 	if (s_quotes % 2 != 0)
// 		return (-1);
// 	if (d_quotes % 2 != 0)
// 		return (1);
// 	return (0);
// }

void	ft_read_line(t_shell *shell)
{
	shell->rawline = readline(PROMPT_MSG);
	if (shell->rawline && ft_strlen(shell->rawline) !=0)
		add_history(shell->rawline);
	if (unclosed_quotes(shell)) // check_quote se è da fare void con exit
		ft_exit(shell, UNCLOSED_QUOTES_ERROR);
	//print_str(shell->quote_idx);
}

