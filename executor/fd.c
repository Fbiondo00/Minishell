/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:47:03 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/02 23:43:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// reset fd originali(con dup2), salvati in shell
// non facciamo chiusura dei dup temp_.. perche servono per il comandi successivi.
// possibile che questa funzione vada usata quando incontriamo AND e OR, PIPE
void ft_reset_original_fd(t_node *node)
{
    dup2(node->shell->temp_input, STDIN_FILENO);
    dup2(node->shell->temp_output, STDOUT_FILENO);
    dup2(node->shell->temp_error, STDERR_FILENO);
}

// V2 is_first viene determinato dentro la funzione
// il nodo di input è il nodo storage, ovvero un nodo non attuale
// ma che conserva gli fd per il nodo attuale
int ft_fd_sub_level2(t_node *node, int lvl, int *is_first)
{
    int i;

    i = -1;
    printf("lvl:%d|is_first:%d\n", lvl, *is_first);
    printf("in ft_fd_sub_level...\n");
    while (++i < node->content.kv_size)
    {
        if ( lvl > 0 && node->content.redir[i].lvl == lvl)
        {
            if ((*is_first == 0 && (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)) || (node->content.redir[i].key == R_OUTPUT_APPEND || node->content.redir[i].key == R_OUTPUT_TRUNC))
            if (node->content.redir[i].key == R_OUTPUT_TRUNC)
            {
                if (ft_open_file(node, i) == 0)
                {
                    printf("ERRORE>ft_open_file\n");
                    return (0);
                }
                node->content.redir[i].key = R_OUTPUT_APPEND;
                if (ft_open_file(node, i) == 0)
                {
                    printf("ERRORE>ft_open_file\n");
                    return (0);
                }
            }
                node->content.redir[i].key = R_OUTPUT_APPEND;
            printf("lvl:%d|ft_fd_sub_level2: FAft_open_file!\n", lvl);
            if (ft_open_file(node, i) == 0)
            {
                printf("ERRORE>ft_open_file\n");
                return (0);
            }
        }
        if (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)
            (*is_first)++;
    }
    return (1);
}

// esegue la redir del lvl indicato.
// nel caso si trattasse di redir di input, o anche here_doc(?)
// esegue solo se is_first == 1
// se si tratta di output esegue sempre.
// ritorna 1 se ok, 0 se errore
// problema:bisogna guardare anche i nodi di eventuali lvl superiori
// perche la redir va fatta su tutti i nodi di questo livello, ma sulla base
// delle indicazioni settate su un altro nodo, potenzialmente non di questo lvl
int ft_fd_sub_level(t_node *node, int lvl, int is_first)
{
    int i;

    i = -1;
    // printf("lvl:%d|is_first:%d\n", lvl, is_first);
    // printf("in ft_fd_sub_level...\n");
    while (++i < node->content.kv_size)
    {
        if (node->content.redir[i].lvl == lvl && (is_first == 0 && (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)))
        {
            // printf("lvl:%d|ft_fd_sub_level: FAft_open_file!\n", lvl);
            if (ft_open_file(node, i) == 0)
                return (0);
        }
    }
    return (1);
}

// esegue tutte le redir con lvl 0(cmd_level)
// ritorna 1 se ok, 0 se errore
// check if (node->content.redir)
int ft_fd_cmd_level(t_node *node)
{
    int i;

    i = -1;
    // printf("in ft_fd_cmd_level...\n");
    while (++i < node->content.kv_size)
    {
        if (node->content.redir[i].lvl == 0)
        {
            printf("ft_fd_cmd_level: FAft_open_file!\n");
            if (!ft_open_file(node, i))
            {
                printf("errore> ft_open_file\n");
                return (0);
            }
                
        }
    }
    return (1);
}

// 0 errore, 1 ok
int norm_exit_status(t_node *node, int i)
{
    if (i == 1)
    {
        node->shell->exit_status = 0;
        return (1);
    }
    else
    {
        node->shell->exit_status = 1;
        ft_reset_original_fd(node);
        return (0);
    }
}

// V1 per execute v1
// esegue in ordine:
// 1. ft_fd_sub_level
// 2. ft_fd_cmd_level
// se ok setta tali valori in std_...
// se ko fa il reset dei valori originali std_..
int ft_do_redir(t_node *node)
{
    int flag;
    int max_lvl;

    flag = 0;
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    // printf("max_lvl:%d\n", max_lvl);
    // esegue tutti gli fd cmd_level in ordine decrescente, non esegue lo zero
    while (max_lvl > 0)
    {
        // printf("max_lvl:%d\n", max_lvl);
        if (ft_fd_sub_level(node, max_lvl--, flag) == 0)
        {
            printf("esce in ft_fd_sub_level...\n");
            ft_reset_original_fd(node);
            return (norm_exit_status(node, 0));
        }
        flag++;
    }
    // printf("PRE: ft_fd_cmd_level(node)\n");
    if (!ft_fd_cmd_level(node))
    {
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 0));
    }
    return (norm_exit_status(node, 1));
}

// V2 utilizzare solo per multi_cmd
// esegue in ordine:
// 1. hidden_out if !NULL
// 2. ft_fd_sub_level
// 3. ft_fd_cmd_level
// se ok setta tali valori in std_...
// se ko fa il reset dei valori originali std_..
int ft_do_redir2(t_node *node, char **arredir)
{
    int fd;
    int flag;

    t_node *storage;

    flag = 0;
    printf("IN ft_do_redir2...\n");
    storage = fd_storage(node);
    printf("Post fd_storage...\n");
    printf("nodo storage:%p\n", storage);
    // questo clean exit solo se fallisce una redir in input, invalida
    // tutta la subshell.. se invece accade a cmd_lvl, invalida solo il cmd
    // ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
    printf("PRE arredir:%p\n", *arredir);
    if (*arredir != NULL)
    {
        printf("arredir: not null\n");
        if (ft_open(&fd, *arredir, R_OUTPUT_APPEND) == 0)
            return (norm_exit_status(node, 0));
        printf("arredir aperto\n");
    }
    printf("PRE: ft_fd_sub_level...\n");
    if (storage->content.redir && ft_fd_sub_level2(storage, node->back->lvl_subshell, &flag) == 0)
    {
        printf("esce in ft_fd_sub_level...\n");
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 0));
    }
    printf("PRE: ft_fd_cmd_level...\n");
    if (!ft_fd_cmd_level(node))
    {
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 0));
    }
    return (norm_exit_status(node, 1));
}

int ft_do_redir2_pipe(t_node *node, char **arredir)
{
    int fd;
    int flag;

    t_node *storage;

    flag = 0;
    printf("IN ft_do_redir2...\n");
    storage = fd_storage(node);
    printf("Post fd_storage...\n");
    printf("nodo storage:%p\n", storage);
    // questo clean exit solo se fallisce una redir in input, invalida
    // tutta la subshell.. se invece accade a cmd_lvl, invalida solo il cmd
    // ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
    printf("PRE arredir:%p\n", *arredir);
    if (*arredir != NULL)
    {
        printf("arredir: not null\n");
        if (ft_open(&fd, *arredir, R_OUTPUT_APPEND) == 0)
            return (norm_exit_status(node, 0));
        printf("arredir aperto\n");
    }
    printf("PRE: ft_fd_sub_level...\n");
    if (storage->content.redir && ft_fd_sub_level2(storage, node->back->lvl_subshell, &flag) == 0)
    {
        printf("esce in ft_fd_sub_level...\n");
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 0));
    }
    printf("PRE: ft_fd_cmd_level...\n");
    return (norm_exit_status(node, 1));
}

// V2 versione per single_cmd
int ft_do_redir3(t_node *node, char **ignore)
{
    int flag;
    int max_lvl;
    (void)ignore;
    flag = 0;
    
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    printf("max_lvl:%d\n", max_lvl);
    // esegue tutti gli fd cmd_level in ordine decrescente, non esegue lo zero
    while (max_lvl > 0)
    {
        // printf("max_lvl:%d\n", max_lvl);
        if (ft_fd_sub_level2(node, max_lvl--, &flag) == 0)
        {
            printf("ERRORE: esce in ft_fd_sub_level...\n");
            ft_reset_original_fd(node);
            return (norm_exit_status(node, 0));
        }
    }
    printf("PRE: ft_fd_cmd_level(node)\n");
    if (ft_fd_cmd_level(node) == 0)
    {
        printf("ERRORE: esce in ft_fd_cmd_level...\n");
        ft_reset_original_fd(node);
        return (norm_exit_status(node, 0));
    }
    return (norm_exit_status(node, 1));
}

// 4 func