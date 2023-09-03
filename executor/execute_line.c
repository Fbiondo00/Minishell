/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/03 01:03:22 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void execute_recursive(t_node *node, char **arredir);

// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
void	ft_single_cmd2(t_node *node, char **arredir, int (*f)(t_node *, char **))
{
	int status;
	pid_t pid;

	printf("in single_cmd...\n");
	if (f(node, arredir) == 0)
	{
		printf("ERRORE>ft_do_redir\n");
		node->shell->exit_status = 1;
		ft_reset_original_fd(node);
		return;
	}
	printf("dopo fd_redir...\n");
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
}

t_node *fork_execute(t_node *node, char **arredir)
{
	pid_t pid;
	int status;

	printf("IN fork_execute...\n");
	if (*arredir != NULL && !is_redir_out(fd_storage(node), node->lvl_subshell))
		*arredir = sign_str(node, node->lvl_subshell);
	pid = fork();
	if (pid == 0)
	{
		node->shell->lvl_subshell++;
		execute_recursive(node, arredir);
	}
	else
	{
		// siamo nel padre
		waitpid(pid, &status, 0);
		node->shell->exit_status = WSTOPSIG(status);
	}
	return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
}


// coincide con la parte del multi_cmd dell attuale execute
// esegue tutta la sequenza di nodi_cmd del suo stesso lvl
// si blocca se finisce la riga
t_node *just_execute(t_node *temp, char **arredir)
{
	t_node *node;
	t_node *next_node;
	t_node *prev_node; // backlog and_or logic

	// printf("IN just_execute...\n");
	node = temp;
	prev_node = NULL;
	while (1)
	{
		printf("--- START GIRO LOOP ---\n");
		if (!node || node->done_lock == 1)
		{
			if (node)
				printf("node->done_lock:%d\n", node->done_lock);
			else
				printf("!node: ritorno:next_cmd2(prev_node->shell, prev_node)\n");
			printf("prev_node:%p\n", prev_node);
			printf("next_cmd2(prev_node->shell, prev_node):%p\n", next_cmd2(prev_node->shell, prev_node));
			return (next_cmd2(prev_node->shell, prev_node));
		}
		else if (!node->content.cmd || !node->content.cmd[0])
		{
			printf("NODE_CMD WITH ONLY REDIR...\n");
			if (ft_do_redir2(node, arredir) == 0)
				printf("errore durante redir!\n");
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
			next_node = ft_do_pipe(node, arredir);
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
			next_node = ft_do_and_or(node, prev_node, arredir);
			node->done_lock = 1;
			if (next_node != NULL)
				prev_node = node;
			printf("AFTER:AND_OR|next_node:p%p\n", next_node);
			printf("POST:AND_OR|prev_node:p%p\n", prev_node);
		}
		node = next_node;
	}
}

// attenzione: prima di forkare, bisogna controllare and_or
// se è da eseguire
void execute_recursive(t_node *node, char **arredir)
{
	t_node *temp;
	t_node *next_node;

	temp = node;
	printf("IN execute_recursive...\n");
	while (1)
	{   // questo è il caso in cui è finita la stringa
		// se !node, puo essere anche che non abbiamo ancora chiuso le subshell..
		// quindi check se si è ancora in subshell.. controllando se lvl_subshell > 0
		if (!node || node->done_lock == 1) 
		{// è ultimo nodo, possiamo essere in subshell oppure no
			ft_reset_original_fd(temp->shell->tree);
			if (node && node->done_lock == 1)
			{
				printf("node->done_lock:%d\n", node->done_lock);
				if (node->back->lvl_subshell < node->shell->lvl_subshell)
				{
					printf("esce da execute...\n");
					printf("node->back->lvl_subshell < node->shell->lvl_subshell\n");
					printf("clean_exit...\n");
					// caso in cui è finita subshell
					//  fa il close ed il free, set a NULL.. simile a ft_remove_heredoc
					if (*arredir != NULL)
						ft_free_hidden_redir(arredir);
				}
				else
				{
					printf("in else, non esce da execute..perche?\n");
					ft_reset_original_fd(node->shell->tree);
					return ;
				}
			}
		}
		// just execute fa tutto il ciclo fino a che non aumenta lvl_subshell del nodo
		if (node->back->lvl_subshell == node->shell->lvl_subshell)
		{
			printf("entra in just_execute\n");
			next_node = just_execute(node, arredir);
		}
		else if (node->back->lvl_subshell > node->shell->lvl_subshell)
		{
			printf("entra in fork_execute\n");
			next_node = fork_execute(node, arredir);
		}
		else if (node->back->lvl_subshell < node->shell->lvl_subshell)
		{
			printf("node->back->lvl_subshell < node->shell->lvl_subshell\n");
			printf("clean_exit...\n");
			// caso in cui è finita subshell
			//  fa il close ed il free, set a NULL.. simile a ft_remove_heredoc
			if (*arredir != NULL) 
				ft_free_hidden_redir(arredir);
			ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
		}
		node = next_node;
		}
}

// coincide con la parte iniziale dell attuale execute
void execute_single_cmd(t_node *node)
{
	ft_do_heredoc(node);
	printf("dopo:ft_do_heredoc(node);\n");
	ft_single_cmd2(node, NULL, ft_do_redir3);
	printf("dopo:ft_single_cmd2(node, NULL, ft_do_redir3);\n");
}

// simile a quello attuale solo che fa anche le redir_sub_lvl
// nell ordine corretto && le subshell
void executeV2(t_shell *shell)
{
	char *arredir;

	arredir = NULL;
	printf("------------------|FASE: EXECUTOR|------------------\n");
	if(is_node_cmd(shell->tree))
	{
		printf("execute_single_cmd(shell->tree);\n");
		execute_single_cmd(shell->tree);
	}
	else
	{
		printf("execute_recursive(go_to_starter_node(shell->tree->left), NULL);\n");
		ft_do_heredoc(go_to_starter_node(shell->tree->left));
		execute_recursive(go_to_starter_node(shell->tree->left), &arredir);
	}
}

// 6 func