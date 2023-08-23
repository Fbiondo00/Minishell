/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 22:02:21 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/21 23:30:55 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
    temp->done_lock = 1;
    next_node = NULL;
    // printf("-----------!CERCO: ..next_cmd_same_lvl...--------------\n\n");
    while (1)
    {
        next_node = next_cmd2(node->shell, temp);
        if (temp == next_node)
            return (NULL);
        else
            temp = next_node;
        // next_node->done_lock = 1;
        // -- print --
        // printf("printing next node ......\n");
        // printf("shell->lvl_subshell:%d|node->lvl_subshell:%d\n", next_node->shell->lvl_subshell, node->lvl_subshell);
        // if (!next_node)
        //     printf("next_node: (null)\n");
        // if (next_node)
        // {
        //     printf("next_node:%p\n", next_node);
        //     printf("check se questo nodo è corretto....\n");
        //     // print_node(node->shell, next_node);
        // }
        // -----------
        if (!next_node)
            break;
        if (next_node && next_node->shell->lvl_subshell == node->lvl_subshell)
        {
            // printf("trovato primo nodo stesso lvl.address:%p\n", next_node);
            return (next_node);
        }
    }
    return (NULL);
}

// dato un nodo, ritorna il prossimo nodo allo stesso lvl_subshell
// in sequenza che sia and o or.
// utilizza: next_cmd_same_lvl
t_node *go_next_cmd_and_or(t_node *node)
{
    // t_node *temp;
    t_node *next_node;

    // temp = node;
    next_node = NULL;
    // printf("-----------!CERCO: ..go_next_cmd_and_or...--------------\n\n");
    while (1)
    {
        next_node = next_cmd_same_lvl(node);
        // temp = next_node;
        if (!next_node)
            break;
        if (next_node->content.op != PIPE)
        {
            // printf("trovato primo nodo stesso lvl.address:%p\n", next_node);
            return (next_node);
        }
    }
    return (NULL);
}
