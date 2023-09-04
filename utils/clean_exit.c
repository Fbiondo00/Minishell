/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/02 01:14:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/04 03:10:14 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void ft_free_str(char **str)
{
	if (*str)
	{
		free(*str);
		*str = NULL;
	}
}

void norm_remove_heredoc(t_shell *shell)
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

void remove_node(t_node *node)
{   
	printf("removing node:%p...\n", node);
	ft_free_str(&node->raw_cmd);
	ft_free_str(&node->quote_idx);
	//content(che non va friato)
	if (node->content.cmd != NULL)
		free_matrix(node->content.cmd);
	if (node->content.redir != NULL)
	{
		ft_free_str(&node->content.redir->value);
		free(node->content.redir);
	}
	free(node);
}

void recursive_remove(t_node *node)
{
	if (node->left != NULL)
	{
		recursive_remove(node->left);
		recursive_remove(node->right);
	}	
	else
		remove_node(node);
}

void remove_tree(t_shell *shell)
{
	printf("removing tree...\n");
	if (is_node_cmd(shell->tree))
	   remove_node(shell->tree);
	else
	{
	   recursive_remove(shell->tree);
	   remove_node(shell->tree);
	}
	shell->tree = NULL;
	printf("tree removed!\n");
}

void just_reset(t_shell *shell)
{
	shell->error = 0;
	shell->exit_builtin = 0;
	shell->exit_status = 0;
	shell->lvl_subshell = 0;
	// printf("PRE:shell->rawline:%p\n", shell->rawline);
	ft_free_str(&shell->rawline);
	// printf("AFTER:shell->rawline:%p\n", shell->rawline);
	// printf("PRE:shell->quote_idx:%p\n", shell->quote_idx);
	ft_free_str(&shell->quote_idx);
	// printf("AFTER:shell->quote_idx:%p\n", shell->quote_idx);
	// printf("PRE:shell->str:%p\n", shell->rawline);
	ft_free_str(&shell->str);
	// printf("AFTER:shell->str:%p\n", shell->rawline);
	if (shell->tree != NULL)
	{
		norm_remove_heredoc(shell);
		remove_tree(shell);
	}
}

void ft_reset_original_fd_null(t_shell *shell)
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
//  V2
void ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit)
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
	{
		// ft_reset_original_fd(node); // ??
		free_envp(shell);
		just_reset(shell);
		close(shell->temp_input);
		close(shell->temp_output);
		close(shell->temp_error);
		exit(exit_status);
	}
}




// to_exit == 1 se bisogna fare anche exit
// to_exit == 0 per fare solo clean
// problema del set a NULL, da gestire come in ft_free_hidden_redir ?
// FARE IL RESET ANCHE DEL LVL_SUBSHELL e di tutte le altre variabili!
// o chiamare shell_init ancora??
// void ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit)
// {

// 	if (shell->quote_idx)
// 	{
// 		printf("shell->quote_idx:%p\n", shell->quote_idx);
// 		free(shell->quote_idx);
// 		shell->quote_idx = NULL;
// 	}
// 	if (shell->rawline)
// 	{
// 		printf("shell->rawline:%p\n", shell->rawline);
// 		free(shell->rawline);
// 		shell->rawline = NULL;
// 	}
// 	if (shell->str)
// 	{
// 		printf("shell->str:%p\n", shell->rawline);
// 		free(shell->str);
// 		shell->str = NULL;
// 	}
// 	if (shell->tree)
// 	{
// 		if (is_node_cmd(shell->tree))
// 		{
// 			printf("ft_remove_heredoc(shell->tree);\n");
// 			ft_remove_heredoc(shell->tree);
// 		}
// 		else
// 		{
// 			printf("ft_remove_heredoc(go_to_starter_node(shell->tree->left));\n");
// 			ft_remove_heredoc(go_to_starter_node(shell->tree->left));
// 		}
// 	}
// 	// if (shell->tree)
// 	// free_tree(shell);  friare tutto l albero navigazione nodi, poi free.
// 	// mi perdo l exit status se free, salvare in var locale?
// 	if (str) // cazzata exit_status??
// 		shell->exit_status = write(2, str, ft_strlen(str)) - ft_strlen(str) + exit_status;
// 	printf("fine line: shell->exit_status:%d\n", shell->exit_status);
// 	if (to_exit) // dovrebbe essere chiamata solo da builtin exit..
// 	{
// 		free_envp(shell);
// 		// ft_reset_original_fd(); ??

// 		close(shell->temp_input);
// 		close(shell->temp_output);
// 		close(shell->temp_error);
// 		exit(exit_status);
// 	}
// }
