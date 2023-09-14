/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 02:08:33 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void execute_recursiveV2(t_node *node);

// esegue la redirection dell output della subshell.
// se op del nodo è AND/OR stampa l output
// se op del nodo è PIPE fornisce output come input next_cmd
void ft_do_redir_fork(t_node *node, t_shell *shell)
{
	char *str;
	
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

t_node *fork_executeV2(t_node *node)
{
	pid_t pid;
	int fd[2];
	int status;
	t_node *resume_node;

	resume_node = NULL;
	if (node->shell->lvl_subshell == 0)
	{
		if (pipe(fd) == -1)
			perror("pipe");
	}
	pid = fork();
	if (pid == 0)
	{
		if (node->shell->lvl_subshell == 0)
		{
			close(fd[0]);
			node->shell->new_temp_output = fd[1];
		}
		node->shell->lvl_subshell++;
		execute_recursiveV2(node);
	}
	else
	{
		waitpid(pid, &status, 0);
		node->shell->exit_status = WSTOPSIG(status);
		resume_node = next_cmd2(node->shell, last_cmd_same_lvl(node));
		if (node->shell->lvl_subshell == 0)
		{
			close(fd[1]);
			node->shell->new_temp_input = fd[0];
			ft_do_redir_fork(resume_node, node->shell);
		}
	}
	return (resume_node);
}



t_node *just_executeV2(t_node *temp)
{
	t_node *node;
	t_node *next_node;
	t_node *prev_node;

	node = temp;
	prev_node = NULL;
	next_node = NULL;
	while (1)
	{
		if (temp->shell->lvl_subshell >0)
			dup2(temp->shell->new_temp_output, STDOUT_FILENO);
		if (!node->content.cmd || !node->content.cmd[0])
		{
			if (ft_get_op(node) == PIPE ||(!prev_node && node == go_to_starter_node(node->shell->tree->left)) || (prev_node && norm_check(node)) || (!prev_node && node != go_to_starter_node(node->shell->tree->left) && norm_check(node)))
			{
				if (ft_do_redir2(node) == 1)
				{
					return (next_cmd_same_lvl(node));
				}
				if (ft_do_redir2(node) == 2)
				{
					return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
				}
				node->done_lock = 1;
				next_node = next_cmd_same_lvl(node);
				if (next_node != NULL)
					prev_node = node;
			}
			else
			{
				return (next_cmd_same_lvl(node));
			}
		}
		else if (ft_get_op(node) == PIPE)
		{
			next_node = ft_do_pipe(node);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
		}
		else if (ft_back_node(node)->lvl_subshell > node->shell->lvl_subshell && node != go_to_starter_node(node->shell->tree->left) && ft_back_node(node)->content.op == PIPE)
		{
			next_node = wrap_ft_single_cmd2(node);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
		}
		else if (ft_get_op(node) == AND || ft_get_op(node) == OR )
		{
			next_node = ft_do_and_or(node, prev_node);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
		}
		node = next_node;
		if (!node || node->done_lock == 1)
		{
			if (node)
			{
				return (node);
			}
			else
			{
				return (NULL);
			}
		}
		return (next_node);
	}
}

t_node *wrap_ft_single_cmd2(t_node *node)
{
	int res;

	res = ft_single_cmd2(node, ft_do_redir2);
	if (res == 1)
	{
		return (next_cmd_same_lvl(node));
	}
	if (res == 2)
	{ 
		ft_clean_exit(node->shell, NULL, 34, 1);
	}
	return (next_cmd_same_lvl(node));
}

void execute_recursiveV2(t_node *node)
{
	t_node *temp;
	t_node *next_node;

	temp = node;
	while (1)
	{
		if ((node != NULL && next_cmd_same_lvl(node) && node->is_last == 1) || ( node && node->is_last == 1 && !next_cmd_same_lvl(node) && node->done_lock == 0))
		{
			ft_dup2(&node->shell->new_temp_output, STDOUT_FILENO);
			if (ft_get_op(node) == PIPE || (node == go_to_starter_node(node->shell->tree->left)) || (norm_check(node)))
				ft_single_cmd2(node, ft_do_redir2);
			ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
		} 
		if (!node || node->done_lock == 1 || (node && ft_back_node(node)->lvl_subshell < node->shell->lvl_subshell  && node->shell->lvl_subshell != 0))
		{ 
			if (!node)
			{
				while (temp->shell->lvl_subshell > 0)
					ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
				if (temp->shell->lvl_subshell == 0)
					return;
			}
			else 
			{
				ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
			}
			if ((node && node->done_lock == 1 && ft_back_node(node)->lvl_subshell >= node->shell->lvl_subshell && node->shell->lvl_subshell != 0))
			{
				ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
			}
			else
			{
				return;
			}
		}
		if (ft_back_node(node)->lvl_subshell == node->shell->lvl_subshell || node->is_last == 1 || node->is_last == 2)
		{
			next_node = just_executeV2(node);
			node->shell->can_flag++;
		}
		else if (ft_back_node(node)->lvl_subshell > node->shell->lvl_subshell && node->is_last == 0)
		{
			next_node = fork_executeV2(node);
		}
		node = next_node;
	}
}

void fix_out_trunc(t_node *node)
{
	int i;
	int fd;

	i = -1;
	if (node->content.redir == NULL)
	{
		return;
	}
	while (++i < node->content.kv_size)
    {
		if (node->content.redir[i].lvl > 0 && node->content.redir[i].key == R_OUTPUT_TRUNC)
		{
			fd = open(node->content.redir[i].value, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0777);
			close (fd);
			node->content.redir[i].key = R_OUTPUT_APPEND;
		}
	}
}

// se presente un node_op con lvl subshell < 0, lo setta a 0
void fix_redir(t_node *node)
{
	t_node *temp;
	t_node *next_node;

	temp = node;
	next_node = NULL;
	while (1)
	{
		fix_out_trunc(temp);
		next_node = next_cmd2(node->shell, temp);
		if (!next_node)
			return;
		temp = next_node;
	}
}

// se presente un node_op con lvl subshell < 0, lo setta a 0
void fix_lvl_subshell(t_node *node)
{
	t_node *temp;
	t_node *next_node;

	temp = node;
	next_node = NULL;
	while (1)
	{
		if (temp->back->lvl_subshell < 0)
			temp->back->lvl_subshell = 0;
		next_node = next_cmd2(node->shell, temp);
		if (!next_node)
			return;
		temp = next_node;
	}
}

void executeV2(t_shell *shell)
{
	if(is_node_cmd(shell->tree))
	{
		execute_single_cmd(shell->tree);
	}
	else
	{
		fix_redir(go_to_starter_node(shell->tree->left));
		fix_lvl_subshell(go_to_starter_node(shell->tree->left));
		ft_do_heredoc(go_to_starter_node(shell->tree->left));
		execute_recursiveV2(go_to_starter_node(shell->tree->left));
	}
}

// dato un nodo ritorna l op corrispettivo
// se è left -> back
// se è right -> back -> back
// se è last -> back
// non dovrebbe mai tornare 0
int ft_get_op(t_node *node)
{
	if (!next_cmd2(node->shell, node))
	{
		return (node->back->content.op);
	}
	if (is_left_branch(node))
	{
		return (node->back->content.op);
	}
	if (!is_left_branch(node))
	{
		return (node->back->back->content.op);
	}
	return (0);
}

t_node *ft_back_node(t_node *node)
{
	if (!next_cmd2(node->shell, node))
	{
		return (node->back);
	}
	if (is_left_branch(node))
	{
		return (node->back);
	}
	if (!is_left_branch(node))
	{	
		if (node->back->back->right == node->back)
		{
			if (node->back->back->back)
				return (node->back->back->back);
			else
				return (node->back);
		}
		return (node->back->back);
	}
	return (0);
}
