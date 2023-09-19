/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 15:07:43 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_recursivev2(t_node *node);

void	norm_node(t_node **node, t_node **prev_node, t_node *next_node)
{
	(*node)->done_lock = 1;
	if (next_node != NULL)
		*prev_node = *node;
}

// esegue la redirection dell output della subshell.
// se op del nodo è AND/OR stampa l output
// se op del nodo è PIPE fornisce output come input next_cmd
void	ft_do_redir_fork(t_node *node, t_shell *shell)
{
	char	*str;

	if (!node || ft_get_op(node) == AND || ft_get_op(node) == OR)
	{
		str = get_next_line(shell->new_temp_input);
		while (str != NULL)
		{
			printf("%s", str);
			ft_free_str(&str);
			str = get_next_line(shell->new_temp_input);
		}
	}
	else
	{
		dup2(shell->new_temp_input, STDIN_FILENO);
		node->flag_pipe = 1;
	}
	close(shell->new_temp_input);
}

void	norm_father(t_node **node, t_node **resume_node, int *status, int pid)
{
	waitpid(pid, status, 0);
	(*node)->shell->exit_status = WSTOPSIG(*status);
	*resume_node = next_cmd2((*node)->shell, last_cmd_same_lvl(*node));
}

void	norm_if_child_fork(t_node **node, int fd[2])
{
	if ((*node)->shell->lvl_subshell == 0)
	{
		close(fd[0]);
		(*node)->shell->new_temp_output = fd[1];
	}
	(*node)->shell->lvl_subshell++;
	execute_recursivev2(*node);
}

t_node	*fork_executev2(t_node *node)
{
	pid_t	pid;
	int		fd[2];
	int		status;
	t_node	*resume_node;

	resume_node = NULL;
	if (node->shell->lvl_subshell == 0)
		if (pipe(fd) == -1)
			perror("pipe");
	pid = fork();
	if (pid == 0)
		norm_if_child_fork(&node, fd);
	else
	{
		norm_father(&node, &resume_node, &status, pid);
		if (node->shell->lvl_subshell == 0)
		{
			close(fd[1]);
			node->shell->new_temp_input = fd[0];
			ft_do_redir_fork(resume_node, node->shell);
		}
	}
	return (resume_node);
}
