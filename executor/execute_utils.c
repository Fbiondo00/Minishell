/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 23:32:44 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 01:17:39 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// ovvero senza la condizione sul pipe
int norm_checkV2skip(t_node *node)
{
    if ((ft_get_op(node) == AND && ok_status(node) == 1) ||
        (ft_get_op(node) == OR && ok_status(node) == 0))
        return (1);
    else
        return (0);
}

int norm_check(t_node *node)
{
    if ((node->back->content.op == PIPE) || (node->back->content.op == AND && ok_status(node) == 1) ||
        (node->back->content.op == OR && ok_status(node) == 0))
        return (1);
    else
        return (0);
}

// ritorna 1 se è ultimo  node della pipe-chain
int is_last_pipe(t_node *node)
{
    if (next_cmd_same_lvl(node) && ft_get_op(next_cmd_same_lvl(node)) == PIPE)
        return (0);
    return (1);
}

char *sign_str(t_node *node, int lvl)
{
    char *str;
    char *alf;
    (void)node;

    alf = "ABCDEFGHILMNOPQRSTUVZabcdefghilmnopqrstuvz9876543210";
    str = ft_strjoin("lvl", alf + ft_strlen(alf) - 1 - lvl);
    return (str);
}

// ritorna l ultimo cmd appartenente allo stesso lvl
// se non esiste un next_cmd ritorna il nodo di input
// di fatto non torna mai NULL
t_node *last_cmd_same_lvl(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    while (1)
    {
        next_node = next_cmd_same_lvl(temp);
        if (!next_node)
        {
            return (temp);
        }
        if (next_node->is_last == 1)
        {
            return (next_node);
        }
        temp = next_node;
    }
    return (NULL);
}

// ritorna 1 se nel node storage  indicato è presente
int is_redir_out(t_node *node, int lvl)
{
    int i;

    i = -1;
    while (++i < node->content.kv_size)
    {
        if (node->content.redir[i].lvl == lvl && (node->content.redir[i].key == R_OUTPUT_APPEND || node->content.redir[i].key == R_OUTPUT_TRUNC))
            return (1);
    }
    return (0);
}

// 5 func