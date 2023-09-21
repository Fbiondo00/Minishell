/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 15:53:44 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/20 22:32:29 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	norm_while_here(t_node *node, char *str, int i)
{
	return ((ft_strncmp(str, node->content.redir[i].value,
				ft_strlen(str) - 1, 0)) || (ft_strlen(str) - 1)
		!= ft_strlen(node->content.redir[i].value));
}

void	norm_here_doc1(t_node *node, char **str, int i, int fd)
{
	write(1, &">", 1);
	*str = get_next_line(0);
	if ((ft_strncmp(*str, node->content.redir[i].value, ft_strlen(*str) - 1, 0))
		&& write(fd, *str, ft_strlen(*str)) == -1)
		perror("Write error");
}

void	norm_child_pipe(t_node **node, int fd[2])
{
	if ((*node)->shell->lvl_subshell > 0)
		if (ft_do_redir2_pipe(*node) == 2)
			ft_clean_exit((*node)->shell, NULL, 34, 1);
	if ((*node)->flag_pipe == 0 || (*node)->is_last == 2)
	{
		if (next_cmd2((*node)->shell, *node))
		{
			close(fd[0]);
			ft_dup2(&fd[1], STDOUT_FILENO);
		}
		else
			dup2((*node)->shell->temp_output, STDOUT_FILENO);
	}
	if (ft_fd_cmd_level(*node) == 1)
		ft_clean_exit((*node)->shell, NULL, 1, 1);
	if (is_builtin(*node))
		exit(execute_builtin(*node, (*node)->shell));
	else
		ft_execve(*node);
}

void	norm_pipe_father(t_node **node, int *status, int pid, int fd[2])
{
	waitpid(pid, status, 0);
	(*node)->shell->exit_status = WSTOPSIG(*status);
	if ((*node)->shell->exit_status == 34)
		ft_clean_exit((*node)->shell, NULL, 1, 1);
	if ((*node)->flag_pipe == 0 || (*node)->is_last == 2)
	{
		close(fd[1]);
		ft_dup2(&fd[0], STDIN_FILENO);
	}
}

// n.b1: pipe vince su finto append
// n.b2:refactor simple_cmd  per utilizzarlo
// ....solo la parte dell !is_builtin vareso variabile
// fare v2 che accetta redir2 come input
t_node	*ft_do_pipe(t_node *node)
{
	pid_t	pid;
	int		status;
	int		len;
	int		fd[2];

	len = ft_strlen(node->content.cmd[0]);
	if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
		return (do_exit(node));
	else
	{
		if (node->flag_pipe == 0 || node->is_last == 2)
			if (pipe(fd) == -1)
				perror("pipe");
		pid = fork();
		if (pid == 0)
			norm_child_pipe(&node, fd);
		else
			norm_pipe_father(&node, &status, pid, fd);
	}
	return (next_cmd_same_lvl(node));
}
