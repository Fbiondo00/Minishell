/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line_OLD.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/04 03:02:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"



// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
void ft_single_cmdOLD(t_node *node)
{
    int     status;
    pid_t   pid;

    printf("in single_cmd...\n");
    if (!ft_do_redir(node))
    {
        printf("!ft_do_redir\n");
        node->shell->exit_status = 1;
        ft_reset_original_fd(node);
        return;
    }
    // printf("OUT\n");
    if (node->content.cmd && node->content.cmd[0])
    {
        // printf("IN\n");
        if (is_builtin(node))
            execute_builtin(node, node->shell);
        else
        {
            // printf("fork..\n");
            // refactor per utilizzo anche in pipe
            pid = fork();
            if (pid == 0)
                ft_execve(node);
            else
            {
                waitpid(pid, &status, 0);
                node->shell->exit_status = WSTOPSIG(status);
                // printf("node->shell->exit_status:%d\n", node->shell->exit_status);
            }
        }
    }
    ft_reset_original_fd(node);
    // printf("POST: ft_reset_original_fd|pid:%d\n",getpid());
}


int norm_checkOLD(t_node *node)
{
    if ((node->back->content.op == AND && ok_status(node) == 1) ||
        (node->back->content.op == OR && ok_status(node) == 0))
        return (1);
    else
        return (0);
}

// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node *ft_do_and_orOLD(t_node* node, t_node *prev_node)
{
    // 1. check status
    // prima di poter eseguire un nodo and o or bisogna controllare exit_status precedente.
    printf("norm_check(node):%d\n", norm_checkOLD(node));
    if (!prev_node  || (prev_node && norm_checkOLD(node)))
    {
        ft_single_cmdOLD(node);
        printf("cmd and_or eseguito....\n");
        if (norm_checkOLD(node) && next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        {
            printf("HERE!\n");
            return (next_cmd_same_lvl(node));
        }  
        else
        {
            printf("return:go_next_cmd_and_or(node)...\n");
            return (go_next_cmd_and_or(node)); // se non devo entrare nella sequenza non ci entro
        }
    }
    // qualora l exit status non permetta l esecuzione del cmd, tale cmd (talvolta insieme ad alcuni altri in caso si tratti delle pipe) vengono skippati.
    // pertanto ritorno il prossimo nodo utile per l esecuzione.
    node->done_lock = 1;
    printf("IN AND_OR:go_next_cmd_and_or\n\n");
    return (go_next_cmd_and_or(node));
}



// n.b1: pipe vince su finto append
// n.b2:refactor simple_cmd  per utilizzarlo 
// ....solo la parte dell !is_builtin vareso variabile
t_node *ft_do_pipeOLD(t_node* node)
{
    
    pid_t pid;
    int status;
    int len;
    int fd[2];
    
    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
    { // fork in tutti i casi tranne exit.
        printf("eseguo exit...\n");
        if (ft_do_redir(node) == 0)
        {
            printf("errore durante redir!\n");
            return (next_cmd_same_lvl(node));
        }
        execute_builtin(node, node->shell); // poi va a return next_cmd_same_lvl
    }
    else
    {
        // se è ultimo elemento sequenza pipe, ristabilire stdout e non fare pipe()
        if (!is_last_pipe(node))
        {
            if (pipe(fd) == -1)
                perror("pipe");
        }
        pid = fork();
        if (pid == 0)
        {
            if (!is_last_pipe(node))// && next_cmd_same_lvl(node)->done_lock != 1)
            {
                close(fd[0]);
                ft_dup2(&fd[1], STDOUT_FILENO);
            }
            else
                dup2(node->shell->temp_output, STDOUT_FILENO);
            if (ft_do_redir(node) == 0)
                exit(33);
            if (is_builtin(node))
                exit(execute_builtin(node, node->shell));
            else
                ft_execve(node);
        }
        else
        {
            if (!is_last_pipe(node))
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
    return(next_cmd_same_lvl(node));
}



void executeOLD(t_shell *shell)
{
    t_node *node;
    t_node *next_node;
    t_node *prev_node; //backlog and_or logic

    prev_node = NULL;
    printf("------------------|FASE: EXECUTOR|------------------\n");
    if (is_node_cmd(shell->tree))
    {
        printf("nodo op è assente...\n");
        ft_do_heredoc(shell->tree);
        ft_single_cmdOLD(shell->tree);
        return ;
    }
    // printf(" è presente almeno 1 nodo op...\n");
    node = go_to_starter_node(shell->tree->left);
    ft_do_heredoc(node);
    while (1)
    {
        // if (node->back->lvl_subshell > 0) // non va messo come primo: seg fault
        //     ft_do_subshell(node);
        if (!node || node->done_lock == 1)
        {
            if (node)
                printf("node->done_lock:%d|esce da execute...\n", node->done_lock);
            ft_reset_original_fd(shell->tree);
            return ;
        }
        else if (!node->content.cmd && !node->content.cmd[0])
        {
            // printf("NODE_CMD WITH ONLY REDIR...\n");
            ft_do_redir(node);
            // printf("PRE:JUST_REDIR|node:p%p\n", node);
            // printf("PRE:JUST_REDIR|prev_node:p%p\n", prev_node);
            prev_node = node;
            next_node = next_cmd_same_lvl(node);
            // printf("POST:JUST_REDIR|prev_node:p%p\n", prev_node);
            // printf("POST:JUST_REDIR|next_node:p%p\n", next_node);
        }
         if (node->back->content.op == PIPE)
        {
            // printf("ft_do_pipe...\n");
            // printf("PRE:PIPE|prev_node:p%p\n", prev_node);
            next_node = ft_do_pipeOLD(node);
            prev_node = node;
            // printf("PRE:PIPE|node:p%p\n", node);
            // printf("POST:PIPE|next_node:p%p\n", next_node);
            // printf("POST:PIPE|prev_node:p%p\n", prev_node);
        }
        else if (node->back->content.op == AND || node->back->content.op == OR)
        {
            // printf("ft_do_and_or...\n");
            // printf("PRE:AND_OR|node:p%p\n", node);
            // printf("PRE:AND_OR|prev_node:p%p\n", prev_node);
            next_node = ft_do_and_orOLD(node, prev_node);
            prev_node = node;
            // printf("AFTER:AND_OR|next_node:p%p\n", next_node);
            // printf("POST:AND_OR|prev_node:p%p\n", prev_node);
        }
        node = next_node;
    }
}

void	print_node(t_shell *shell, t_node *node)
{
	int	i;

	i = -1;
	if (shell && shell->tree->content.idx_op != -1)
		printf("\n\nBASH:%s\n", shell->rawline);
	if (shell && shell->tree->content.idx_op == -1)
		printf("\n\nBASH:%s\n", shell->str);
	printf("---------------------------\n");
	printf("----------|NODO|-----------\n");
	printf(">node:%p\n", node);
	while (node->content.cmd[++i])
		printf("cmd[%d]:|%s|len:%d ", i, node->content.cmd[i],
				ft_strlen(node->content.cmd[i]));
	printf("\n");
	printf("-----|REDIRECTION|-----\n");
	if (node->content.redir)
	{
		i = -1;
		printf("node->content.kv_size:%d\n", node->content.kv_size);
		while (++i < node->content.kv_size)
		{
			if (!i)
				printf("-----------------------\n");
			printf("REDIR N°%d\n", i + 1);
			printf("FD:%d| ", node->content.redir[i].fd);
			printf("LVL:%d| ", node->content.redir[i].lvl);
			if (node->content.redir[i].key == 13)
				printf("KEY: R_INPUT| ");
			if (node->content.redir[i].key == 14)
				printf("KEY: R_INPUT_HERE_DOC| ");
			if (node->content.redir[i].key == 15)
				printf("KEY: R_OUTPUT_TRUNC| ");
			if (node->content.redir[i].key == 16)
				printf("KEY: R_OUTPUT_APPEND| ");
			printf("VALUE:{%s}len:%d\n", node->content.redir[i].value,
					ft_strlen(node->content.redir[i].value));
		}
	}
	if (!node->content.redir)
		printf("il nodo non ha redirezioni!\n");
	if (node->back)
	{
		printf("-------|back node|-------\n");
		printf("indirizzo:%p\n", node->back);
		if (node->back->content.op == 10)
			printf("operatore: AND\n");
		if (node->back->content.op == 11)
			printf("operatore: OR\n");
		if (node->back->content.op == 12)
			printf("operatore: PIPE\n");
		printf("lvl_subshell:%d\n", node->back->lvl_subshell);
		if (node->back->back)
		{
			printf("----|back>back node|----\n");
			printf("indirizzo:%p\n", node->back->back);
			if (node->back->back->content.op == 10)
				printf("operatore: AND\n");
			if (node->back->back->content.op == 11)
				printf("operatore: OR\n");
			if (node->back->back->content.op == 12)
				printf("operatore: PIPE\n");
			printf("lvl_subshell:%d\n", node->back->back->lvl_subshell);
		}
		if (!node->back->back)
			printf("node->back->back:(null)!!\n");
	}
	if (!node->back)
		printf("node->back:(null)!!\n");
	printf("------|==FINE_NODO==|------\n");
	printf("---------------------------\n");
}