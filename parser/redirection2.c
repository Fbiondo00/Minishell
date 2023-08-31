/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 17:12:26 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/31 22:52:16 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// i: indice del primo operatore (anche in caso di << >>)
// num: index della redir
// la logica dovrebbe essere semplice: per ogni )
// prima della redir: lvl++; (parte da 0)
// setta il lvl relativa alla redir salvata, prima o dopo set_fd?
// (cat || echo b) <test
// (echo b && (echo c)<2)>p)
void	set_redir_op(t_node *node, int idx, int num)
{
	if (node->raw_cmd[idx] == '>')
	{
		if (node->raw_cmd[idx - 1] == '>')
			node->content.redir[num].key = R_OUTPUT_APPEND;
		else
			node->content.redir[num].key = R_OUTPUT_TRUNC;
	}
	if (node->raw_cmd[idx] == '<')
	{
		if (node->raw_cmd[idx - 1] == '<')
			node->content.redir[num].key = R_INPUT_HERE_DOC;
		else
			node->content.redir[num].key = R_INPUT;
	}
}

// controlla se è presente fd valido prima della redir
int	is_fd_valid(t_node *node, int idx)
{
	while (--idx > 0)
	{
		if (node->raw_cmd[idx] == ' ')
			break ;
		if (!ft_isdigit2(node->raw_cmd[idx]))
			return (0);
	}
	return (1);
}

// idx: indice del primo operatore (anche in caso di << >>)
// num: index della redir
// modifica valore di node->content.fd:
// setta -1: in caso di assenza o in caso di valore non valido(non numero)
// setta x: in caso di numero valido
// nel secondo caso sostituisce il valore con il char space
void	set_fd(t_node *node, int idx, int num)
{
	set_lvl(node, idx, num);
	if (node->raw_cmd[idx - 1] != ' ' && is_fd_valid(node, idx))
	{
		if (!ft_isdigit2(node->raw_cmd[idx - 2]))
			node->content.redir[num].fd = node->raw_cmd[idx - 1] - 48;
		else
			node->content.redir[num].fd = -1;
		while (--idx >= 0)
		{
			if (node->raw_cmd[idx] == ' ')
				break ;
			node->raw_cmd[idx] = ' ';
		}
	}
	else
		node->content.redir[num].fd = -1;
}

// ritorna il numero di operatori presenti nella raw_cmd
// soluzione: utilizza raw_cmd ed in_quotes()
int	get_rd_size(t_node *node)
{
	int	i;
	int	count_redir;

	i = -1;
	count_redir = 0;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
		{
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
				i++;
			count_redir++;
		}
		if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
		{
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
				i++;
			count_redir++;
		}
	}
	return (count_redir);
}

char	*mody_str(char *str, int *k)
{
	char	*new_str;

	*k = 0;
	new_str = malloc(ft_strlen(str) - 1);
	new_str[ft_strlen(str) - 1] = '\0';
	return (new_str);
}
