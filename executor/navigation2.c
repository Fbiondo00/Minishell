/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 22:02:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 01:29:44 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// e elimina tutti gli here_doc creati nell' albero
void ft_remove_heredoc(t_node *node)
{
    int i;
    char *path;
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    while (1)
    {
        i = -1;
        while (++i < temp->content.kv_size)
        {
            if (temp->content.redir[i].key == R_INPUT_HERE_DOC)
            {
                path = ft_strjoin("./", temp->content.redir[i].value);
                if (unlink(path) != 0)
                    perror("unlink error");
                free(path);
            }
        }
        next_node = next_cmd2(temp->shell, temp);
        if (!next_node)
            return;
        else
            temp = next_node;
    }
}

// ritorna il prossimo nodo, a parità di livello
// shell->lvl_subshell;
// si scorre tutto l albero fino a che non trova un nodo
// tale che: node->lvl_subshell == shell->lvl_subshell;
// il nodo da ritornare è node->right. (verifica affermazione)
t_node *next_cmd_same_lvl(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    while (1)
    {
        next_node = next_cmd2(temp->shell, temp);
        if (!next_node || next_node->done_lock == 1)
        {
            return (NULL);
        }
        else if (ft_back_node(next_node)->lvl_subshell >= ft_back_node(temp)->lvl_subshell) 
        {
            return (next_node);
        }
        else if (ft_back_node(next_node)->lvl_subshell < ft_back_node(temp)->lvl_subshell && next_node->is_last == 1)
        {
            return (next_node);
        }
        temp = next_node;
    }
    return (NULL);
}

// dato un nodo, ritorna il prossimo nodo allo stesso lvl_subshell
// in sequenza che sia and o or.
// utilizza: next_cmd_same_lvl
// bug temp not used??
t_node *go_next_cmd_and_or(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    while (1)
    {
        temp->done_lock = 1;
        next_node = next_cmd_same_lvl(temp);
        temp = next_node;
        if (!next_node)
            break;
        if (ft_get_op(next_node) != PIPE && node->back != next_node->back)
        {
            return (next_node);
        }
    }
    return (NULL);
}

// 3 func