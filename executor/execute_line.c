/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/04 05:39:06 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void execute_recursive(t_node *node, char **arredir);
void execute_recursiveV2(t_node *node);

// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
int	ft_single_cmd2(t_node *node, int (*f)(t_node *))
{
	int status;
	int res;
	pid_t pid;

	printf("in single_cmd...\n");
	res = f(node);
	if (res != 0)
	{
		printf("ERRORE>ft_do_redir\n");
		ft_reset_original_fd(node);
		return(res);
	}
	// printf("dopo fd_redir...\n");
	if (node->content.cmd && node->content.cmd[0])
	{
		if (is_builtin(node))
			execute_builtin(node, node->shell);
		else
		{
			printf("fork..\n");
			// refactor per utilizzo anche in pipe
			pid = fork();
			if (pid == 0)
				ft_execve(node);
			else
			{
				waitpid(pid, &status, 0);
				printf("FINE SINGLE_CMD!\n");
				node->shell->exit_status = WSTOPSIG(status);
				// printf("node->shell->exit_status:%d\n", node->shell->exit_status);
			}
		}
	}
	ft_reset_original_fd(node);
	// printf("POST: ft_reset_original_fd|pid:%d\n",getpid());
	return (0);
}


t_node *fork_executeV2(t_node *node)
{
	pid_t pid;
	int status;

	printf("IN fork_execute...\n");
	pid = fork();
	if (pid == 0)
	{
		printf("siamo nel figlio\n");
		node->shell->lvl_subshell++;
		execute_recursiveV2(node);
	}
	else
	{
		waitpid(pid, &status, 0);
		printf("siamo nel padre\n");
		node->shell->exit_status = WSTOPSIG(status);
	}
	return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
}

// coincide con la parte del multi_cmd dell attuale execute
// esegue tutta la sequenza di nodi_cmd del suo stesso lvl
// si blocca se finisce la riga
t_node *just_executeV2(t_node *temp)
{
	t_node *node;
	t_node *next_node;
	t_node *prev_node;

	node = temp;
	prev_node = NULL;
	while (1)
	{
		printf("--- START GIRO LOOP ---\n");
		if (!node || node->done_lock == 1)
		{
			if (node)
			{
				printf("in just_execute..\n");
				printf("node->done_lock:%d\n", node->done_lock);
				ft_reset_original_fd(node->shell->tree);
				// return (NULL);
			}
			else
			{
				printf("in just_execute: NODE == NULL\n");
				// return (NULL);
				// return (next_cmd2(prev_node->shell, prev_node));
			}
			if (prev_node != NULL)
				return (next_cmd2(prev_node->shell, prev_node));
			else
				return (temp);
		}
		else if (!node->content.cmd || !node->content.cmd[0])
		{
			printf("NODE_CMD WITH ONLY REDIR...\n");
			if (ft_do_redir2(node) == 1)
			{
				printf("errore redir CMD_LVL!\n");
				return (next_cmd_same_lvl(node));
			}
			if (ft_do_redir2(node) == 2)
			{
				printf("errore redir SUB_LVL!\n");
				return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
			}
			printf("PRE:JUST_REDIR|node:p%p\n", node);
			printf("PRE:JUST_REDIR|prev_node:p%p\n", prev_node);
			node->done_lock = 1;
			next_node = next_cmd_same_lvl(node);
			if (next_node != NULL)
				prev_node = node;
			printf("POST:JUST_REDIR|prev_node:p%p\n", prev_node);
			printf("POST:JUST_REDIR|next_node:p%p\n", next_node);
		}
		else if (node->back->content.op == PIPE)
		{
			printf("ft_do_pipe...\n");
			printf("PRE:PIPE|prev_node:p%p\n", prev_node);
			next_node = ft_do_pipe(node);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
			printf("PRE:PIPE|node:p%p\n", node);
			printf("POST:PIPE|next_node:p%p\n", next_node);
			printf("POST:PIPE|prev_node:p%p\n", prev_node);
		}
		else if (node->back->content.op == AND || node->back->content.op == OR)
		{
			printf("ft_do_and_or...\n");
			printf("PRE:AND_OR|node:p%p\n", node);
			printf("PRE:AND_OR|prev_node:p%p\n", prev_node);
			next_node = ft_do_and_or(node, prev_node);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
			printf("AFTER:AND_OR|next_node:p%p\n", next_node);
			printf("POST:AND_OR|prev_node:p%p\n", prev_node);
		}
		node = next_node;
	}
}

// V2 senza hidden
void execute_recursiveV2(t_node *node)
{
	t_node *temp;
	t_node *next_node;

	temp = node;
	printf("IN execute_recursiveV2...\n");
	while (1)
	{
		if (!node || node->done_lock == 1)
		{ // è ultimo nodo, possiamo essere in subshell oppure no
			if (!node)
				printf("NON C E NODO!!\n");
			else
			{
				printf("C E NODO!!\n");
				printf("node->done_lock:%d\n", node->done_lock);
				printf("node-> back->lvl_subshell:%d\n", node->back->lvl_subshell);
				printf("node->shell->lvl_subshell:%d\n", node->shell->lvl_subshell);
			}
			if (node && node->done_lock == 1 && node->back->lvl_subshell >= node->shell->lvl_subshell && node->shell->lvl_subshell != 0)
			{
				printf("node->done_lock:%d\n", node->done_lock);
				printf("USCITA DALLA SUBSHELL!\n");
				ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
			}
			else
			{
				printf("NON SIAMO IN SUBSHELL, RITORNA NEXT_LINE\n");
				return;
			}
		}
		if (node->back->lvl_subshell == node->shell->lvl_subshell)
		{
			printf("entra in just_executeV2\n");
			next_node = just_executeV2(node);
		}
		else if (node->back->lvl_subshell > node->shell->lvl_subshell)
		{
			printf("entra in fork_executeV2\n");
			next_node = fork_executeV2(node);
		}
		node = next_node;
	}
}



// coincide con la parte iniziale dell attuale execute
void execute_single_cmd(t_node *node)
{
	ft_do_heredoc(node);
	printf("dopo:ft_do_heredoc(node);\n");
	ft_single_cmd2(node, ft_do_redir3);
	printf("dopo:ft_single_cmd2(node, NULL, ft_do_redir3);\n");
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
	printf("------------------|FASE: EXECUTOR|------------------\n");
	if(is_node_cmd(shell->tree))
	{
		printf("esegue execute_single_cmd(shell->tree);\n");
		execute_single_cmd(shell->tree);
	}
	else
	{
		// fix_lvl_subshell(go_to_starter_node(shell->tree->left));
		printf("esegue execute_recursive(go_to_starter_node(shell->tree->left), NULL);\n");
		ft_do_heredoc(go_to_starter_node(shell->tree->left));
		execute_recursiveV2(go_to_starter_node(shell->tree->left));
	}
}

// 6 func