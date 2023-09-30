/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 15:20:21 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/23 18:38:48 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	norm_wild(char **new_str, char *tem)
{
	char	*tem2;

	tem2 = ft_strdup(*new_str);
	free(*new_str);
	*new_str = ft_strjoin2(tem2, tem);
	free(tem2);
}

char	*norm_wild_return(char **dirname, DIR **dir, char *new_str, int flag)
{
	if (flag == 1)
	{
		free(*dirname);
		return ((char *)1);
	}
	free(*dirname);
	closedir(*dir);
	return (new_str);
}

void	ft_input(char *new_str, t_node *node, int y)
{
	char	*raw;
	char	*quote;

	if (new_str)
	{
		raw = modify_raw_and_quote2(node, y, new_str, 32);
		ft_free_str(&node->raw_cmd);
		node->raw_cmd = raw;
		quote = modify_raw_and_quote2(node, y, new_str, 48);
		ft_free_str(&node->quote_idx);
		node->quote_idx = quote;
		free(new_str);
	}
}

char	*create_str(t_node *node, int max, int min)
{
	char	*str;
	int		ij;

	ij = 0;
	str = malloc(sizeof(*str) *(max - min + 1));
	if (!str)
		return (NULL);
	while (min < max && (node->raw_cmd[min] != 32 && node->raw_cmd[min]))
		str[ij++] = node->raw_cmd[min++];
	str[ij++] = 0;
	return (str);
}

void	ft_wild(t_node *node, int i, int y)
{
	int		max;
	int		min;
	char	*str;
	char	*new_str;

	new_str = NULL;
	max = ft_max(node, i);
	min = ft_min(node, i);
	if (min < 0)
		min = 0;
	str = create_str(node, max, min);
	if (ft_stronly(str, '*') == 1)
		new_str = ft_one();
	else
		new_str = new_str_1(str, new_str, 0);
	free(str);
	ft_input(new_str, node, y);
}
