/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_utilsget.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 00:38:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/27 00:47:10 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// ritorna la len fino a =
int	get_idx_eq(t_node *node, int idx)
{
	int	i;

	i = -1;
	while (node->shell->env[idx][++i])
	{
		if (node->shell->env[idx][i] == '=')
			break ;
	}
	return (i);
}

int	get_idx_eq_str2(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
	{
		if (str[i] == '=')
			break ;
	}
	return (i);
}

int	get_idx_eq_str(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
	{
		if (str[i] == '=' || str[i] == '+')
			break ;
	}
	return (i - 1);
}

int	get_idx_eq_st2(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
	{
		if (str[i] == '=')
			break ;
	}
	return (i);
}

char	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
		i++;
	return (s1[i] - s2[i]);
}
