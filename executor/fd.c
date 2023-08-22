/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:47:03 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/22 23:17:15 by rdolzi           ###   ########.fr       */
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

// esegue la redir del lvl indicato.
// nel caso si trattasse di redir di input, o anche here_doc(?)
// esegue solo se is_first == 1
// se si tratta di output esegue sempre.
// ritorna 1 se ok, 0 se errore
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
            if (!ft_open_file(node, i))
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
                return (0);
        }
    }
    return (1);
}

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
    // printf("in ft_single_cmd...\n");
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    // printf("max_lvl:%d\n", max_lvl);
    // esegue tutti gli fd cmd_level in ordine decrescente, non esegue lo zero
    while (max_lvl > 0)
    {
        // printf("max_lvl:%d\n", max_lvl);
        if (!ft_fd_sub_level(node, max_lvl--, flag))
        {
            printf("esce in ft_fd_sub_level...\n");
            ft_reset_original_fd(node);
            return (0);
        }
        flag++;
    }
    // printf("PRE: ft_fd_cmd_level(node)\n");
    if (!ft_fd_cmd_level(node))
    {
        ft_reset_original_fd(node);
        return (0);
    }
    return (1);
}

// 4 func