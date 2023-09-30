/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 20:51:33 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/23 21:41:26 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// converte quotes s/d !in_quotes in 2 char scelto
void	convert_quotes(t_node *node, char d)
{
	int	i;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
		if (node->quote_idx[i] == 34 || node->quote_idx[i] == 39)
			node->raw_cmd[i] = d;
}

// cerca un char non utilizzato in rawline
void	ft_safe_char(t_node *node, char *c)
{
	int	i;
	int	y;
	int	count;

	i = 65;
	while (i < 124)
	{
		y = -1;
		count = 0;
		while (++y < ft_strlen(node->raw_cmd))
			if (node->raw_cmd[y] == i)
				count++;
		if (count == 0)
		{
			*c = i;
			return ;
		}
		i++;
	}
}

// per tutte le stringhe, se match il 2 char, fare il remove
void	ft_remove_quotes(t_node *node, int idx, char d)
{
	int		i;
	int		j;
	int		k;
	char	*str;

	i = -1;
	while (++i < ft_strlen(node->content.cmd[idx]))
	{
		if (node->content.cmd[idx][i] == d)
		{
			j = -1;
			k = 0;
			str = malloc(ft_strlen(node->content.cmd[idx]));
			str[ft_strlen(node->content.cmd[idx]) - 1] = '\0';
			while (++j < ft_strlen(node->content.cmd[idx]))
			{
				if (j == i)
					k++;
				str[j] = node->content.cmd[idx][k++];
			}
			free(node->content.cmd[idx]);
			node->content.cmd[idx] = str;
			i--;
		}
	}
}

void	reset_space(t_node *node, int idx, char c)
{
	int	i;

	i = -1;
	while (++i < ft_strlen(node->content.cmd[idx]))
		if (node->content.cmd[idx][i] == c)
			node->content.cmd[idx][i] = ' ';
}

void	ft_do_cmd(t_node *node)
{
	int		i;
	char	c;
	char	d;

	ft_safe_char(node, &c);
	convert_space(node, c);
	ft_safe_char(node, &d);
	convert_quotes(node, d);
	node->content.cmd = ft_split(node->raw_cmd, ' ');
	if (!node->content.cmd || !node->content.cmd[0])
		return ;
	if (c != d)
	{
		i = -1;
		while (node->content.cmd[++i])
			reset_space(node, i, c);
	}
	i = -1;
	while (node->content.cmd[++i])
		ft_remove_quotes(node, i, d);
}

// 6 func