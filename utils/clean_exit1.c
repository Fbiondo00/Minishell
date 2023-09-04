/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_exit1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 16:43:01 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/04 16:46:11 by flaviobiond      ###   ########.fr       */
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
		printf("ft_remove_heredoc(shell->tree);\n");
		ft_remove_heredoc(shell->tree);
	}
	else
	{
		printf("ft_remove_heredoc(go_to_starter_node(shell->tree->left));\n");
		ft_remove_heredoc(go_to_starter_node(shell->tree->left));
	}
}

void	remove_node(t_node *node)
{
	printf("removing node:%p...\n", node);
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
