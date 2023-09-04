/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 23:40:11 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/04 03:45:28 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// restituisce il nodo dove sono locate le redir delle sublvl
// ovvero ritorna il nodo tale che lvl_subshell < node_lvl
// se si viene incontrato un nodo di lvl superiore all attuale, va avanti
t_node *fd_storage(t_node *node)
{
    t_node *temp;
    t_node *nxt;

    temp = node;
    nxt = NULL;
    printf("IN fd_storage...\n");
    if (node == node->shell->tree)
        return (node);
    else
    {
        while (1)
        {
            nxt = last_cmd_same_lvl(temp);
            printf("last_cmd_same_lvl(temp):%p\n", last_cmd_same_lvl(temp));
            temp->done_lock = 0;
            if (!next_cmd2(nxt->shell, nxt))
                return (nxt);
            if (next_cmd2(nxt->shell, nxt)->back->lvl_subshell < nxt->shell->lvl_subshell)
                return (nxt);
            temp = next_cmd2(nxt->shell, nxt);
        }
    }
}

// fa il close ed il free, set a NULL.. simile a ft_remove_heredoc
void ft_free_hidden_redir(char **arredir)
{
    printf("IN ft_free_hidden_redir...\n");
    char *path;
    path = ft_strjoin("./", *arredir);
    if (unlink(path) != 0)
        perror("unlink error");
    free(path);
    printf("Removed file:%s\n", *arredir);
    free(*arredir);
    *arredir = NULL;
}

// n.b1: pipe vince su finto append
// n.b2:refactor simple_cmd  per utilizzarlo
// ....solo la parte dell !is_builtin vareso variabile
// fare v2 che accetta redir2 come input
t_node *ft_do_pipe(t_node *node)
{

    pid_t pid;
    int status;
    int len;
    int fd[2];

    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
    { // fork in tutti i casi tranne exit.
        printf("eseguo exit...\n");
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
            // ordine:
            // 1 sub_lvl
            // 2 pipe
            // 3 cmd_lvl
            if (ft_do_redir2_pipe(node) != 0)
                exit(34);
            if (!is_last_pipe(node)) // && next_cmd_same_lvl(node)->done_lock != 1)
            {
                close(fd[0]);
                ft_dup2(&fd[1], STDOUT_FILENO);
            }
            else
            {
                dup2(node->shell->temp_output, STDOUT_FILENO);
                // if (ft_do_redir2_pipe(node) == 0)
                //     exit(34);
            }
            if (!ft_fd_cmd_level(node))
            {
                ft_reset_original_fd(node);
                norm_exit_status(node, 1);
                exit(33);
            }
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
    return (next_cmd_same_lvl(node));
}

// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node *ft_do_and_or(t_node *node, t_node *prev_node)
{
    int res;
    // 1. check status
    // prima di poter eseguire un nodo and o or bisogna controllare exit_status precedente.
    printf("PRE:norm_check(node):%d\n", norm_check(node));
    if ((!prev_node && node == go_to_starter_node(node->shell->tree->left)) || (prev_node && norm_check(node)) || (!prev_node && node != go_to_starter_node(node->shell->tree->left) && norm_check(node)))
    {
        if (!prev_node)
            printf("!prev_node\n");
        printf("start esecuzione comando....\n");
        res = ft_single_cmd2(node, ft_do_redir2);
        if (res == 1)
        {
            printf("errore redir CMD_LVL!\n");
            return (next_cmd_same_lvl(node));
        }
        if (res == 2)
        {
            printf("errore redir SUB_LVL!\n");
            node->done_lock = 1;
            return (node);
        }
        printf("cmd and_or eseguito....\n");
        printf("AFTER:norm_check(node):%d\n", norm_check(node));
        if (norm_check(node))
        {
            printf("HERE!\n");
            // printf("next_cmd_same_lvl(node):%p\n", next_cmd_same_lvl(node));
            return (next_cmd_same_lvl(node));
        }
        else if (!norm_check(node) && next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        {
            printf("return:go_next_cmd_and_or(node)...\n");
            return (go_next_cmd_and_or(node)); // se non devo entrare nella sequenza non ci entro
        }
    }
    // qualora l exit status non permetta l esecuzione del cmd, tale cmd (talvolta insieme ad alcuni altri in caso si tratti delle pipe) vengono skippati.
    // pertanto ritorno il prossimo nodo utile per l esecuzione.
    printf("IN AND_OR:go_next_cmd_and_or\n\n");
    return (go_next_cmd_and_or(node));
}

// 5 func