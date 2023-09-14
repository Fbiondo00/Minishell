/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_cmd.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 22:31:54 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 01:15:59 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// MODIFICA INCOMING...
// echo d && (echo a && (echo b && echo c <u)>p)>g OK out: no_such_file (p:b g:a)
// soluzione: fare sub_lvl_redir se lvl != 0 e farlo del lvl_subshell attuale + 1
// se non esiste lvl_sub_attuale + 1 allora fare solo del lv

int get_max_lvl(t_node *node, t_node *stg)
{
    if (node->shell->lvl_subshell == 0 || !stg->content.redir)
    {
        return (0);
    }
    if (ft_back_node(stg)->lvl_subshell > ft_back_node(node)->lvl_subshell)
    {
        return (ft_back_node(node)->lvl_subshell + 1);
    }
    if (ft_back_node(stg)->lvl_subshell == ft_back_node(node)->lvl_subshell && stg->is_last != 1)
    {
        if (ft_back_node(node)->lvl_subshell - 1 > 0)
            return (ft_back_node(node)->lvl_subshell - 1);
        else
            return (ft_back_node(node)->lvl_subshell);
    }
    if (stg->is_last == 1)
    {
        if ( (node->shell->lvl_subshell - 1) > 0)
        {
            if (!next_cmd_same_lvl(node) || ft_back_node(next_cmd_same_lvl(node))->lvl_subshell <= node->shell->lvl_subshell)
            {
                return (node->shell->lvl_subshell);
            }
            return (node->shell->lvl_subshell - 1);
        }
        else if (node->shell->lvl_subshell - 1 == 0)
        {
            if (node->shell->lvl_subshell == 1 && (!next_cmd_same_lvl(node) || ft_back_node(next_cmd_same_lvl(node))->lvl_subshell <= 1))
            {
                return (node->shell->lvl_subshell);
            }     
            else
            {
                return (node->shell->lvl_subshell + 1);
            }
               
        }
        return (0);
    }
    return (0);
 }

// ritorna 1 se presente
int lvl_is_present(t_node *stg, int lvl)
{
    int i;

    i = -1;
    while (++i < stg->content.kv_size)
        if (stg->content.redir[i].lvl == lvl)
            return (1);
    return (0);
}


// V2 utilizzare solo per multi_cmd
// esegue in ordine:
// 2. ft_fd_sub_level
// 3. ft_fd_cmd_level
// se ok setta tali valori in std_...
// se ko fa il reset dei valori originali std_..
// return 2 se ko sub_lvl
// return 1 se ko cmd_lvl
// return 0 se tutto ok
int ft_do_redir2(t_node *node)
{
    int flag;
    int mx_lv;
    t_node *storage;

    flag = 0;
    storage = fd_storage(node);
    mx_lv = get_max_lvl(node, storage);
    if (node->shell->lvl_subshell > 1 && mx_lv >= 1)
    {
        mx_lv--;
        while (mx_lv <= node->shell->lvl_subshell)
        {
            if (!lvl_is_present(storage, mx_lv))
                mx_lv++;
            else
                break;
        }
        if (storage->content.redir && node->shell->lvl_subshell > 0)
        {
            flag = 0;
            if (ft_fd_sub_level2(storage, mx_lv, &flag) == 0)
            {
                ft_reset_original_fd(node);
                return (norm_exit_status(node, 2));
            }
        } 
    }
    else
    {
        if (storage->content.redir && node->shell->lvl_subshell > 0)
        {
            flag = 0;
            if (ft_fd_sub_level2(storage, mx_lv, &flag) == 0)
            {
                ft_reset_original_fd(node);
                return (norm_exit_status(node, 2));
            }
        }
    }
    if (ft_fd_cmd_level(node) == 1)
    {
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 1));
    }
    return (norm_exit_status(node, 0));
}

int ft_do_redir2_pipe(t_node *node)
{
    int flag;

    t_node *storage;

    flag = 0;
    storage = fd_storage(node);
    if (storage->content.redir && ft_fd_sub_level2(storage, node->shell->lvl_subshell, &flag) == 0)
    {
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 2));
    }
    return (norm_exit_status(node, 0));
}

// V2 versione per single_cmd
int ft_do_redir3(t_node *node)
{
    int flag;
    int max_lvl;

    flag = 0;
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    while (max_lvl > 0)
    {
        if (ft_fd_sub_level2(node, max_lvl--, &flag) == 0)
        {
            ft_reset_original_fd(node);
            return (norm_exit_status(node, 2));
        }
    }
    if (ft_fd_cmd_level(node) == 1)
    {
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 1));
    }
    return (norm_exit_status(node, 0));
}

// esegue il singolo cmd(sia single_cmd che multi_cmd)
// ft_do_redir3       -> single_cmd
// ft_do_redir2       -> multi_cmd (and_or)
// ft_do_redir2_pipe  -> multi_cmd (pipe)
// ritorna 0 se ok
// ritorna 1 se ko cmd_lvl
// ritorna 2 se ko sub_lvl
int ft_single_cmd2(t_node *node, int (*f)(t_node *))
{
    int res;
    int status;
    pid_t pid;

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
            {
                waitpid(pid, &status, 0);
                node->shell->exit_status = WSTOPSIG(status);
            }
        }
    }
    ft_reset_original_fd(node);
    return (0);
}

void execute_single_cmd(t_node *node)
{
    ft_do_heredoc(node);
    ft_single_cmd2(node, ft_do_redir3);
}
