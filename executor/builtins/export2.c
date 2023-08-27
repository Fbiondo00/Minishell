/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 01:29:36 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/27 03:58:35 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ftcheck11(t_node *node)
{
	int	i;
	int	y;

	i = 0;
	y = -1;
	while (node->content.cmd[++i])
	{
		while (node->content.cmd[i][++y])
		{
			if ((node->content.cmd[i][y] >= '0'
				&& node->content.cmd[i][y] <= '9')
				|| (node->content.cmd[i][y] >= 'a'
					&& node->content.cmd[i][y] <= 'z')
				|| (node->content.cmd[i][y] >= 'A'
					&& node->content.cmd[i][y] <= 'Z')
				|| node->content.cmd[i][y] == '='
				|| node->content.cmd[i][y] == '+')
				continue ;
			else
				return (1);
		}
	}
	return (0);
}

int	ft_check(t_node *node)
{
	int	i;

	i = 1;
	while (node->content.cmd[i])
	{
		if ((node->content.cmd[i][0] >= 'a' && node->content.cmd[i][0] <= 'z')
			|| (node->content.cmd[i][0] >= 'A'
				&& node->content.cmd[i][0] <= 'Z'))
			++i;
		else
			return (write(1, "checkexpo\n", 11) - 10);
	}
	if (ftcheck11(node) == 1)
		return (write(1, "checkexpo1\n", 11) - 10);
	return (0);
}
