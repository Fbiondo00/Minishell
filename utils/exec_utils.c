/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:56:05 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 00:35:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_clean_exit1(t_shell *shell, int exit_status)
{
	free_envp(shell);
	just_reset(shell);
	close(shell->temp_input);
	if (shell->new_temp_output != -1)
	{
		close(shell->new_temp_output);
		shell->new_temp_output = -1;
	}
	if (shell->new_temp_input != -1)
	{
		close(shell->new_temp_input);
		shell->new_temp_input = -1;
	}
	close(shell->temp_output);
	close(shell->temp_error);
	exit(exit_status);
}

// ritorna 1, se status ok ritorna 0 se status ko
// 1. check status
// prima di poter eseguire un nodo and o or
// bisogna controllare exit_status precedente.
// uno stato di non errore è:
// se -1, perche è valore default
// se 0, perche un cmd ha fatto exit_success
// rischio perche non guardo se PIPE, ma il nodo dovrebbe essere
// stato correttamente filtrato in execute()
int	ok_status(t_node *node)
{
	if (node->shell->exit_status == -1 || node->shell->exit_status == 0)
	{
		return (1);
	}
	return (0);
}

int	ft_dup2(int *fd, int arg)
{
	if (dup2(*fd, arg) == -1)
	{
		write(2, "Dup2 error\n", 11);
		return (0);
	}
	close(*fd);
	return (1);
}

int	execute_builtin(t_node *node, t_shell *shell)
{
	int	len;

	len = ft_strlen(node->content.cmd[0]);
	if (!ft_strncmp(node->content.cmd[0], "echo", len, 1))
		ft_echo(node);
	if (!ft_strncmp(node->content.cmd[0], "pwd", len, 1))
		ft_pwd(node);
	else if (!ft_strncmp(node->content.cmd[0], "env", len, 1))
		ft_env(shell);
	else if (!ft_strncmp(node->content.cmd[0], "export", len, 1))
		ft_export(shell, node);
	else if (!ft_strncmp(node->content.cmd[0], "unset", len, 1))
		ft_unset(node, shell);
	else if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
		ft_exit(node, shell);
	else if (!ft_strncmp(node->content.cmd[0], "cd", len, 1))
		ft_cd(node, shell);
	return (node->shell->exit_status);
}

// ritorna 1 se è un comando builtin
// ritorna 0 in caso contrario
int	is_builtin(t_node *node)
{
	int	len;

	len = ft_strlen(node->content.cmd[0]);
	if (!ft_strncmp(node->content.cmd[0], "echo", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "pwd", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "env", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "export", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "unset", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
		return (1);
	else if (!ft_strncmp(node->content.cmd[0], "cd", len, 1))
		return (1);
	return (0);
}
