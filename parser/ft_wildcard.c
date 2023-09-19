/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/23 01:45:48 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/18 17:13:27 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_middle(char *str, int index)
{
	int	i;
	int	y;

	y = index;
	i = ft_strlen(str);
	while (++index < i)
		if (str[index] == '*')
			break ;
	while (--y > 0)
		if (str[y] == '*')
			break ;
	if (str[y] == '*' && str[index] == '*')
		return (1);
	return (0);
}

char	*ft_one(void)
{
	char			*tem;
	char			*new_str;
	struct dirent	*entry;
	char			*dirname;
	DIR				*dir;

	new_str = NULL;
	dirname = getcwd(0, 0);
	dir = opendir(dirname);
	if (dir == NULL)
		return (norm_wild_return(&dirname, &dir, new_str, 1));
	entry = readdir(dir);
	while ((entry))
	{
		tem = ft_strjoin2(entry->d_name, " ");
		if (!new_str)
			new_str = ft_strjoin2(tem, " ");
		else
			norm_wild(&new_str, tem);
		free(tem);
		entry = readdir(dir);
	}
	return (norm_wild_return(&dirname, &dir, new_str, 0));
}

int	ft_max(t_node *node, int i)
{
	int	max;

	max = i;
	while (node->raw_cmd[max])
	{
		if (node->raw_cmd[max] == 32 || (node->raw_cmd[max] == 39
				|| node->raw_cmd[max] == 34))
			break ;
		max++;
	}
	max--;
	return (max);
}

int	ft_min(t_node *node, int i)
{
	int	min;

	min = i;
	while (node->raw_cmd[min])
	{
		if (node->raw_cmd[min] == 32 || (node->raw_cmd[min] == 39
				|| node->raw_cmd[min] == 34))
			break ;
		min--;
	}
	min++;
	return (min);
}
