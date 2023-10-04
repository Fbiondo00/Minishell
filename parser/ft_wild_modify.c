/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wild_modify.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 17:58:33 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/31 18:03:17 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	mod_raw_a_qu2(t_node *node, int idx)
{
	int	y;

	y = idx + 1;
	while (y < ft_strlen(node->raw_cmd))
	{
		y++;
		if (node->raw_cmd[y] == '$')
		{
			if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34
				|| node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$'
				|| node->raw_cmd[y] == '/')
				return (y);
		}
		else if (node->raw_cmd[y] == ' ')
			return (y);
	}
	return (y);
}

char	*str2(t_node *node, char *str, int idx, char *new_st)
{
	int	i;
	int	j;
	int	y;
	int	tot_len;

	y = mod_raw_a_qu2(node, idx);
	tot_len = tot_len_1(node, str, idx, y);
	i = -1;
	j = -1;
	while (++i < tot_len)
	{
		if (i < idx)
			new_st[i] = node->raw_cmd[i];
		else if (!str && (i >= idx && i < y))
			new_st[i] = ' ';
		else if (!str && !(i >= idx && i < y))
			new_st[i] = node->raw_cmd[y++];
		else if (str && ++j < ft_strlen(str))
			new_st[i] = str[j];
		else if (str && j >= ft_strlen(str) && j++)
			new_st[i] = node->raw_cmd[y++];
	}
	return (new_st);
}

// idx of $
char	*modify_raw_and_quote2(t_node *node, int idx, char *str, char c)
{
	int		i;
	int		y;
	int		tot_len;
	char	*new_st;

	y = mod_raw_a_qu2(node, idx);
	tot_len = tot_len_1(node, str, idx, y);
	new_st = malloc(tot_len + 1);
	new_st[tot_len] = '\0';
	i = -1;
	if (c == 32)
		new_st = str2(node, str, idx, new_st);
	else if (c == 48)
	{
		while (++i < tot_len)
		{
			if (node->raw_cmd[i] == 34)
				new_st[i] = 34;
			else if (node->raw_cmd[i] == 39)
				new_st[i] = 39;
			else
				new_st[i] = 48;
		}
	}
	return (new_st);
}
