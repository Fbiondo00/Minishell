/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 23:40:11 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 17:16:20 by flaviobiond      ###   ########.fr       */
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
    t_node *fix;
    temp = node;
    nxt = NULL;
    
    if (node == node->shell->tree)
        return (node);
    if (node->is_last == 1)
        return (node);
    fix = next_cmd2(node->shell, node);
    if (fix && ft_back_node(next_cmd2(node->shell, node))->lvl_subshell > node->shell->lvl_subshell && node->shell->lvl_subshell == 0)
    {
        return (node);
    }
    else
    {
        while (1)
        {
            nxt = last_cmd_same_lvl(temp);
            temp->done_lock = 0;
            if (!next_cmd2(nxt->shell, nxt))
            {
                return (nxt);
            }
            if (nxt->is_last == 1)
            {
                return (nxt);
            }
            if (next_cmd2(nxt->shell, nxt)->back->lvl_subshell < nxt->shell->lvl_subshell)
            {
                return (nxt);
            }
            temp = next_cmd2(nxt->shell, nxt);
        }
    }
}


// di fatto ritorna nodo solo se fallisce, in caso
// positivo esegue l exit e fa il clean_exit
t_node *do_exit(t_node *node)
{
    if (ft_do_redir2(node) == 1)
    {
        return (next_cmd_same_lvl(node));
    }
    if (ft_do_redir2(node) == 2)
    {
        return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
    }
    execute_builtin(node, node->shell);
    return (NULL);
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
        return (do_exit(node));
    else
    {
        if (node->flag_pipe == 0 || node->is_last == 2)
        {
            if (pipe(fd) == -1)
                perror("pipe");
        }
        pid = fork();
        if (pid == 0)
        {
            if (node->shell->lvl_subshell > 0)
                if (ft_do_redir2_pipe(node) == 2)
                {
                    ft_clean_exit(node->shell, NULL, 34, 1);
                }
            if (node->flag_pipe == 0 || node->is_last == 2)
            {
                if (next_cmd2(node->shell,node))
                {
                    close(fd[0]);
                    ft_dup2(&fd[1], STDOUT_FILENO);
                }
                else
                    dup2(node->shell->temp_output, STDOUT_FILENO);
            }
            if (ft_fd_cmd_level(node) == 1)
                ft_clean_exit(node->shell, NULL, 1, 1);
            if (is_builtin(node))
                exit(execute_builtin(node, node->shell));
            else
                ft_execve(node);
        }
        else
        {
            waitpid(pid, &status, 0);
            node->shell->exit_status = WSTOPSIG(status);
            if (node->shell->exit_status == 34)
            {
                ft_clean_exit(node->shell, NULL, 1, 1);
            }
            if (node->flag_pipe == 0 || node->is_last == 2)
            {
                close(fd[1]);
                ft_dup2(&fd[0], STDIN_FILENO);
            }
        }
    }
    return (next_cmd_same_lvl(node));
}




// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node *ft_do_and_or(t_node *node, t_node *prev_node)
{
    int res;
    t_node *printt;

    printt = NULL;
    if ((!prev_node && node == go_to_starter_node(node->shell->tree->left)) || (prev_node && norm_check(node)) || (!prev_node && node != go_to_starter_node(node->shell->tree->left) && norm_check(node)) || (!prev_node  && norm_check(node)))
    {
        res = ft_single_cmd2(node, ft_do_redir2);
        if (res == 1)
        {
            return (next_cmd_same_lvl(node));
        }
        if (res == 2)
        { 
            ft_clean_exit(node->shell, NULL, 34, 1);
        }
        printt = next_onev2(node);
        return (printt);
    }
    return (go_next_cmd_and_or(node));
}

// 5 func