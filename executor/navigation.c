/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:46:46 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/21 03:49:42 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// dovrebbe essere simile a get_starter_node
// solo che prende un nodo come input
t_node *go_to_starter_node(t_node *node)
{
    t_node *nod;

    nod = node;
    while (nod)
    {
        if (!nod->left)
            break;
        nod = nod->left;
    }
    return (nod);
}

// al momento credo vada utilizzato solo sui nodi_cmd e non op
int is_left_branch(t_node *node)
{
    if (node == node->back->left)
        return (1);
    return (0);
}

int is_node_cmd(t_node *node)
{
    if (node->content.idx_op == -1)
        return (1);
    return (0);
}

// da valutare se puo essere usato anche per right_branch
// gli viene passato sempre un nod
// restituisce il next node.
// ritorna NULL in diversi casi
t_node *next_cmd(t_shell *shell, t_node *node)
{
    printf("IN NEXT_CMD...\n");
    // non dovrebbe essere aggiunto nell OR anche se  node->back == shell->tree ?
    if (node->back == shell->tree || (!is_left_branch(node) &&
        node->back->back == shell->tree))
    {
        printf("return null\n");
        return NULL;
    }
    if (is_left_branch(node))
    {
        printf("return node->back->right\n");
        return (node->back->right);
    }
    else
    {
        printf("NEXT_CMD: IN ELSE \n");
        if (is_node_cmd(node->back->back->right))
        {
            printf("return node->back->back->right\n");
            return (node->back->back->right);
        }
        else if (node->back == node->back->back->right)
        {
            printf("return node->back->back->back->right\n");
            return (node->back->back->back->right);
        }
        else
        {
            printf("return go_to_starter_node(node->back->back->right) & set lvl..\n");
            go_to_starter_node(node->back->back->right)->back->lvl_subshell += node->back->back->lvl_subshell;
            go_to_starter_node(node->back->back->right)->back->lvl_lock = 1;
            return (go_to_starter_node(node->back->back->right));
        }
    }
}

// 4 func