/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   content.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 20:00:20 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/03 02:35:10 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	get_len_value2(t_node *node, char *c, int i, int *flag)
{
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == ' ' || (ft_strchr(c, node->raw_cmd[i])
				&& !in_quotes(node, i)))
			break ;
		else if (node->raw_cmd[i] == 34 && (*flag)++)
		{
			while (++i < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[i] == 34)
					break ;
			}
			if (node->raw_cmd[i - 1] == 34)
				i--;
		}
		else if (node->raw_cmd[i] == 39 && (*flag)++)
		{
			while (++i < ft_strlen(node->raw_cmd))
				if (node->raw_cmd[i] == 39)
					break ;
			if (node->raw_cmd[i - 1] == 39)
				i--;
		}
	}
	return (i);
}

// restituisce il num dei char da mallocare
// se trova " va avanti anche se ci sono gli spazi
// altrimenti si ferma se trova uno spazio o se trova un char di redirection
// echo ciao >q "q"
// echo ciao >q"q
// echo a >b>c smette di contare quando incotra un redir
// echo a >c"bb "c>y
// idx: char dopo spazio, se esiste.
int	get_len_value(t_node *node, int idx)
{
	int		i;
	int		flag;
	char	*chars;

	i = idx - 1;
	flag = 0;
	chars = "><";
	i = get_len_value2(node, chars, i, &flag);
	if (flag > 0)
		return (i - idx - 2);
	else
		return (i - idx);
}

// filtra raw_cmd e quote_idx dopo la redirection
// settando gli spazi in raw_cmd e gli zeri in quote_idx
// finish: o  char spazio o char redir o è finita la raw
// start: è l ultimo char della redir se << o >>
void	set_raw_cmd_and_quote_idx(t_node *node, int start, int finish)
{
	int		i;
	char	*chars;

	i = -1;
	chars = "><";
	if (ft_strchr(chars, node->raw_cmd[start - 1]))
		start--;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (i >= start && i < finish)
		{
			node->raw_cmd[i] = ' ';
			node->quote_idx[i] = 48;
		}
	}
}

void	set_content(t_node *node)
{
	set_redirection(node);
}

// 3 func