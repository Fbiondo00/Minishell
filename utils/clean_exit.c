/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/02 01:14:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 15:09:13 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	recursive_remove(t_node *node)
{
	if (node->left != NULL)
	{
		recursive_remove(node->left);
		recursive_remove(node->right);
	}	
	else
		remove_node(node);
}

void	remove_tree(t_shell *shell)
{
	if (is_node_cmd(shell->tree))
		remove_node(shell->tree);
	else
	{
		recursive_remove(shell->tree);
		remove_node(shell->tree);
	}
	shell->tree = NULL;
}

void	just_reset(t_shell *shell)
{
	shell->error = 0;
	shell->exit_builtin = 0;
	shell->exit_status = 0;
	shell->lvl_subshell = 0;
	ft_free_str(&shell->rawline);
	ft_free_str(&shell->quote_idx);
	ft_free_str(&shell->str);
	if (shell->tree != NULL)
	{
		norm_remove_heredoc(shell);
		remove_tree(shell);
	}
}

void	ft_reset_original_fd_null(t_shell *shell)
{
	dup2(shell->temp_input, STDIN_FILENO);
	dup2(shell->temp_output, STDOUT_FILENO);
	dup2(shell->temp_error, STDERR_FILENO);
}

// se exit_status == 0
//  va pulita tutta la shell, tranne:
//  env
//  exit_status resettato, per corretta esecuzione next line
//  i vari dup2, vanno mantenuti e non resettati
//  pulizia albero approccio top/down, ricorsivo
void	ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit)
{
	if (to_exit == 0)
	{
		if (shell->tree == NULL)
			ft_reset_original_fd_null(shell);
		else
			ft_reset_original_fd(shell->tree);
		just_reset(shell);
		if (exit_status != 0)
			shell->exit_status = exit_status;
	}
	if (str)
		write(2, str, ft_strlen(str));
	if (to_exit == 1)
		ft_clean_exit1(shell, exit_status);
}
