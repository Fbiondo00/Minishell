/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:55:33 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/03 02:10:43 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// fa il free di str
// ritorna NULL se non trova variabile o se variabile senza =
// ritorna nuova str contenente il valore
// il char ritornato NON e'da friare
char	*var_expand(t_node *node, char *str)
{
	int		i;
	char	*new_str;

	i = -1;
	new_str = NULL;
	while (node->shell->env[++i])
	{
		if (!ft_strncmp2(str, node->shell->env[i], get_idx_eq(node, i), 1))
		{
			new_str = node->shell->env[i] + get_idx_eq(node, i) + 1;
			break ;
		}
	}
	free(str);
	return (new_str);
}

// idx corrisponde a index di $
// ritorna la stringa della variabile
// ritorna null se non c e' alcun char attaccato a seguire
char	*find_var(t_node *node, int idx)
{
	int		i;
	int		y;
	int		flag;
	char	*str;

	flag = 0;
	y = idx + 1;
	while (y < ft_strlen(node->raw_cmd) && node->raw_cmd[y] != ' '
		&& node->raw_cmd[y] != 34 && node->raw_cmd[y] != 39
		&& node->raw_cmd[y] != '$' && node->raw_cmd[y] != '/')
	{
		y++;
		flag++;
	}
	if (flag == 0)
		return (NULL);
	str = malloc(flag + 1);
	str[flag] = '\0';
	i = -1;
	while (flag-- > 0)
		str[++i] = node->raw_cmd[++idx];
	return (var_expand(node, str));
}

// espande le variabili.
// se in single quote non espande
// se in double quote espande
// piccolo problema: se fai espansione poi non coincide
// quote_idx con idx del raw_cmd
// soluzione, aggiungere spazi in quote idx e realloc
// $HOME cc /$HOME"  / "$HOME" BUG
// $HOME"ciao" > bash: /Users/rdolziciao: No such file or directory
// SE METTO SOLO 1 " O ' errore free, dopo un primo comando,
//	se lo si fa subito da syntax error.
// "$HOME"  poi "$HOME
// $HOME$HOME  ->COME STOP DELLA PAROLA DA PRENDERE
//	+ COME STOP PER MALLOC NEW_STR
void	ft_do_expand(t_node *node)
{
	int		i;
	char	*raw;
	char	*quote;
	char	*str;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '$' && in_quotes(node, i) != -1)
		{
			str = find_var(node, i);
			raw = modify_raw_and_quote(node, i, str, 32);
			ft_free_str(&node->raw_cmd);
			node->raw_cmd = raw;
			quote = modify_raw_and_quote(node, i, str, 48);
			ft_free_str(&node->quote_idx);
			node->quote_idx = quote;
		}
	}
}

void	ft_do_asterisk(t_node *node)
{
	int	i;
	int	y;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '*' && !in_quotes(node, i))
		{
			y = i;
			while (node->raw_cmd[y])
			{
				if (node->raw_cmd[y] == ' ')
					break ;
				y--;
			}
			y++;
			ft_wild(node, i, y);
		}
	}
}

void	set_cmd(t_node *node)
{
	ft_do_asterisk(node);
	ft_do_question(node);
	ft_do_expand(node);
	ft_do_cmd(node);
	ft_lowercase_cmd(node);
}
