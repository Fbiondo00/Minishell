/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 20:50:42 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/19 00:34:44 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_str(char **str)
{
	if (*str)
	{
		free(*str);
		*str = NULL;
	}
}

void	norm_remove_heredoc(t_shell *shell)
{
	if (is_node_cmd(shell->tree))
	{
		ft_remove_heredoc(shell->tree);
	}
	else
	{
		ft_remove_heredoc(go_to_starter_node(shell->tree->left));
	}
}

void	remove_node(t_node *node)
{
	ft_free_str(&node->raw_cmd);
	ft_free_str(&node->quote_idx);
	if (node->content.cmd != NULL)
		free_matrix(node->content.cmd);
	if (node->content.redir != NULL)
	{
		ft_free_str(&node->content.redir->value);
		free(node->content.redir);
	}
	free(node);
}

int	ft_atoi(const char *str)
{
	int	i;
	int	j;
	int	segn;
	int	check;

	check = 0;
	i = 0;
	j = 0;
	segn = 1;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	while (str[i] == '-' || str[i] == '+')
	{
		if (str[i++] == '-')
			segn = -1;
		check++;
	}
	if (check > 1)
		return (j);
	while (str[i] >= '0' && str[i] <= '9')
	{
		j = j * 10 + (str[i] - '0');
		i++;
	}
	return (j * segn);
}
