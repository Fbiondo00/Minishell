/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 23:40:11 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/30 19:36:42 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// restituisce il nodo dove sono locate le redir delle sublvl
// ovvero ritorna il nodo tale che lvl_subshell < node_lvl
// se si viene incontrato un nodo di lvl superiore all attuale, va avanti
t_node	*fd_storage(t_node *node)
{
	t_node	*temp;
	t_node	*nxt;

	temp = node;
	nxt = NULL;
	if (node == node->shell->tree || node->is_last == 1)
		return (node);
	if (next_cmd2(node->shell, node)
		&& ft_back_node(next_cmd2(node->shell, node))->lvl_subshell
		> node->shell->lvl_subshell && node->shell->lvl_subshell == 0)
		return (node);
	else
	{
		while (1)
		{
			nxt = last_cmd_same_lvl(temp);
			temp->done_lock = 0;
			if (!next_cmd2(nxt->shell, nxt) || nxt->is_last == 1)
				return (nxt);
			if (next_cmd2(nxt->shell, nxt)->back->lvl_subshell
				< nxt->shell->lvl_subshell)
				return (nxt);
			temp = next_cmd2(nxt->shell, nxt);
		}
	}
}

// di fatto ritorna nodo solo se fallisce, in caso
// positivo esegue l exit e fa il clean_exit
t_node	*do_exit(t_node *node)
{
	if (ft_do_redir2(node) == 1)
		return (next_cmd_same_lvl(node));
	if (ft_do_redir2(node) == 2)
		return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
	execute_builtin(node, node->shell);
	return (NULL);
}

// esegue il singolo cmd(sia single_cmd che multi_cmd)
// ft_do_redir3       -> single_cmd
// ft_do_redir2       -> multi_cmd (and_or)
// ft_do_redir2_pipe  -> multi_cmd (pipe)
// ritorna 0 se ok
// ritorna 1 se ko cmd_lvl
// ritorna 2 se ko sub_lvl
int	ft_single_cmd2(t_node *node, int (*f)(t_node *))
{
	int		res;
	int		status;
	pid_t	pid;

	res = f(node);
	if (res != 0)
	{
		ft_reset_original_fd(node);
		return (res);
	}
	if (node->content.cmd && node->content.cmd[0])
	{
		if (is_builtin(node))
			execute_builtin(node, node->shell);
		else
		{
			pid = fork();
			if (pid == 0)
				ft_execve(node);
			else
				norm_else_single_cmd(&node, &status, pid);
		}
	}
	ft_reset_original_fd(node);
	return (0);
}

// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node	*ft_do_and_or(t_node *node, t_node *prev_node)
{
	int		res;
	t_node	*printt;

	printt = NULL;
	if ((!prev_node && node == go_to_starter_node(node->shell->tree->left))
		|| (prev_node && norm_check(node))
		|| (!prev_node && node != go_to_starter_node(node->shell->tree->left)
			&& norm_check(node)) || (!prev_node && norm_check(node)))
	{
		res = ft_single_cmd2(node, ft_do_redir2);
		if (res == 1)
			return (next_cmd_same_lvl(node));
		if (res == 2)
			ft_clean_exit(node->shell, NULL, 34, 1);
		printt = next_onev2(node);
		return (printt);
	}
	return (go_next_cmd_and_or(node));
}
