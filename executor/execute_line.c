/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 00:55:34 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_single_cmd(t_node *node)
{
	int		status;
	pid_t	pid;

	if (!ft_do_redir(node))
	{
		node->shell->exit_status = 1;
		return ;
	}
	if (is_builtin(node))
		execute_builtin(node, node->shell);
	else
	{
		pid = fork();
		if (pid == 0)
			ft_execve(node);
		else
		{
			waitpid(pid, &status, 0);
			node->shell->exit_status = WSTOPSIG(status);
		}
	}
	ft_reset_original_fd(node);
}

void	ft_do_subshell(t_node *node)
{
	(void)node;
}

t_node	*ft_do_and_or(t_node *node)
{
	if (is_left_branch(node) || ((node->back->content.op == AND
				&& ok_status(node) == 1) || (node->back->content.op == OR
				&& ok_status(node) == 0)))
	{
		ft_single_cmd(node);
		return (next_cmd_same_lvl(node));
	}
	node->done_lock = 1;
	return (go_next_cmd_and_or(node));
}

t_node	*ft_do_pipe(t_node *node)
{
	pid_t	pid;
	int		status;
	int		len;
	int		fd[2];

	len = ft_strlen(node->content.cmd[0]);
	if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
	{
		printf("eseguo exit...\n");
		if (!ft_do_redir(node))
		{
			printf("perche qui!\n");
			node->shell->exit_status = 1;
			return (next_cmd_same_lvl(node));
		}
		execute_builtin(node, node->shell);
	}
	else
	{
		if (next_cmd_same_lvl(node)
			&& next_cmd_same_lvl(node)->back->content.op == PIPE)
		{
			if (pipe(fd) == -1)
				perror("pipe");
		}
		pid = fork();
		if (pid == 0)
		{
			if (next_cmd_same_lvl(node)
				&& next_cmd_same_lvl(node)->back->content.op == PIPE
				&& next_cmd_same_lvl(node)->done_lock != 1)
			{
				close(fd[0]);
				ft_dup2(&fd[1], STDOUT_FILENO);
			}
			else
				dup2(node->shell->temp_output, STDOUT_FILENO);
			if (!ft_do_redir(node))
				exit(33);
			if (is_builtin(node))
			{
				execute_builtin(node, node->shell);
				exit(0);
			}
			else
				ft_execve(node);
		}
		else
		{
			if (next_cmd_same_lvl(node)
				&& next_cmd_same_lvl(node)->back->content.op == PIPE)
			{
				close(fd[1]);
				ft_dup2(&fd[0], STDIN_FILENO);
			}
			else
				dup2(node->shell->temp_output, STDOUT_FILENO);
			waitpid(pid, &status, 0);
			node->shell->exit_status = WSTOPSIG(status);
			printf("node->shell->exit_status:%d\n", node->shell->exit_status);
		}
	}
	return (next_cmd_same_lvl(node));
}

void	execute(t_shell *shell)
{
	t_node	*node;
	t_node	*next_node;

	if (is_node_cmd(shell->tree))
	{
		printf("nodo op è assente...\n");
		ft_single_cmd(shell->tree);
		return ;
	}
	printf(" è presente almeno 1 nodo op...\n");
	node = go_to_starter_node(shell->tree->left);
	while (1)
	{
		if (!node || node->done_lock == 1)
		{
			if (node && node->done_lock == 1)
				ft_reset_original_fd(node);
			break ;
		}
		if (node->back && node->back->content.op == PIPE)
		{
			printf("ft_do_pipe...\n");
			next_node = ft_do_pipe(node);
			printf("next_node:p%p\n", next_node);
		}
		else if (node->back && (node->back->content.op == AND
					|| node->back->content.op == OR))
		{
			printf("ft_do_and_or...\n");
			next_node = ft_do_and_or(node);
			printf("next_node:p%p\n", next_node);
		}
		node = next_node;
	}
}

// 5 func