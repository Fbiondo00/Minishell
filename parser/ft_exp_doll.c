/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exp_doll.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 17:50:39 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/23 19:46:42 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	mod_raw_a_qu(t_node *node, int idx)
{
	int	y;

	y = idx + 1;
	while (y < ft_strlen(node->raw_cmd))
	{
		y++;
		if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34
			|| node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$'
			|| node->raw_cmd[y] == '/')
			return (y);
	}
	return (y);
}

int	tot_len_1(t_node *node, char *str, int idx, int y)
{
	int	tot_len;

	if (str)
		tot_len = idx + ft_strlen(str) + ft_strlen(node->raw_cmd) - y;
	if (!str)
		tot_len = ft_strlen(node->raw_cmd);
	return (tot_len);
}

char	*str1(t_node *node, char *str, int idx, char *new_st)
{
	int	i;
	int	j;
	int	y;
	int	tot_len;

	y = mod_raw_a_qu(node, idx);
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
char	*modify_raw_and_quote(t_node *node, int idx, char *str, char c)
{
	int		i;
	int		y;
	int		tot_len;
	char	*new_st;

	y = mod_raw_a_qu(node, idx);
	tot_len = tot_len_1(node, str, idx, y);
	new_st = malloc(tot_len + 1);
	new_st[tot_len] = '\0';
	i = -1;
	if (c == 32)
		new_st = str1(node, str, idx, new_st);
	else if (c == 48)
	{
		while (++i < tot_len && node->raw_cmd[i])
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
