/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   content.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 20:00:20 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 00:38:27 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
// restituisce il num dei char da mallocare
// se trova " va avanti anche se ci sono gli spazi
// altrimenti si ferma se trova uno spazio o se trova un char di redirection
// echo ciao >q "q"
// echo ciao >q"q"

// echo a >b>c smette di contare quando incotra un redir
// echo a >c"bb "c>y
// idx: char dopo spazio, se esiste.
int	get_len_value(t_node *node, int idx)
{
	int		i;
	int		flag;
	char	*chars;

	i = idx;
	flag = 0;
	chars = "><";
	while (i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == ' ' || (ft_strchr(chars, node->raw_cmd[i])
				&& !in_quotes(node, i)))
			break ;
		else if (node->raw_cmd[i] == 34)
		{
			flag++;
			while (++i < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[i] == 34)
					break ;
			}
			if (node->raw_cmd[i - 1] == 34)
				i--;
		}
		else if (node->raw_cmd[i] == 39)
		{
			flag++;
			while (++i < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[i] == 39)
					break ;
			}
			if (node->raw_cmd[i - 1] == 39)
				i--;
		}
		i++;
	}
	if (flag > 0)
		return (i - idx - 2);
	else
		return (i - idx);
}

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
	printf("set_redirection..\n");
	set_redirection(node);
}
