/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navigation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:46:46 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/29 04:16:00 by rdolzi           ###   ########.fr       */
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

// V2: torna il generico next_cmd
// torna null se non trova nulla
// entra che è sempre un node_cmd, esce sempre node_cmd
// t_node *next_cmd2(t_shell *shell, t_node *node)
// {
//     if (shell->tree == node || (!is_left_branch(node) && node->back == shell->tree))
//     {
//         printf("next_cmd2:return NULL\n");
//         return (NULL);
//     }
//     if (is_left_branch(node)) // siamo in un nodo sinistro, ma sempre check if node_op a destra
//     {
//         printf("entriamo in nodo in un nodo SINISTRO, valutazione...\n");
//         if (is_node_cmd(node->back->right))
//         {
//             printf("si tratta di un node_cmd...\n");
//             printf("node_cmd2,return: node->back->right\n");
//             return (node->back->right);
//         } 
//         else
//         {
//             printf("NON si tratta di un node_cmd, ma di NODE_OP...\n");
//             // si tratta di nodo_op, quindi vado a cercare il next_cmd
//             return (go_to_starter_node(node->back->right));
//         }
//     }
//     else
//     {
//         printf("entriamo in nodo in un nodo DESTRO, valutazione...\n");
//         if (is_node_cmd(node->back->back->right))
//         {
//             printf("si tratta di un node_cmd...\n");
//             printf("node_cmd2,return: node->back->right\n");
//             return (node->back->back->right);
//         }
//         else
//         {
//             printf("ELSE,RETURN:go_to_starter_node(node->back->back->right))\n");
//             return (go_to_starter_node(node->back->back->right));
//         }
            
//     }
// }

// entra che è sempre un node_cmd, esce sempre node_cmd
t_node *next_cmd2(t_shell *shell, t_node *node)
{
    t_node* to_check;
    if (shell->tree == node)// || (!is_left_branch(node) && node->back == shell->tree))
    {
        // printf("next_cmd2:return NULL(shell->tree == node)\n");
        return (NULL);
    }
    if (is_left_branch(node)) // siamo in un nodo sinistro, ma sempre check if node_op a destra
    {
        // printf("nodo SINISTRO, valuto cosa tornare...\n");
        if (is_node_cmd(node->back->right))
        {
            // printf("right node è node_cmd...\n");
            // printf("node_cmd2,return: node->back->right\n");
            return (node->back->right);
        }
        else
        {
            // printf("NON si tratta di un node_cmd,torno l origine ...\n");
            // si tratta di nodo_op, quindi vado a cercare il next_cmd
            return (go_to_starter_node(node->back->right));
        }
    }
    else // siamo in un nodo destro, sicuramente è nodo_cmd.
    { // torno 
        // printf("entriamo in nodo in un nodo DESTRO, valutazione...\n");
        if (node->back->back)
        {
            // printf("E' presente node->back->back...\n");
            // printf("node->back->back:%p\n", node->back->back);
            if (!node->back->back->back || !node->back->back->right) // a&&b|c
            {
                // printf("RETURN NULL:(!node->back->back->back && !node->back->back->right...\n");
                return (NULL);
            }
            // check del bivio particolare...
            if (node->back->back->right == node->back)
            {

                // printf("to_check = node->back->back->back->right;\n");
                to_check = node->back->back->back->right;
            }
            else
            {
                // printf("to_check = to_check = node->back->back->right;\n");
                to_check = node->back->back->right;
            }
            // printf("controllo se to_check è nodo_cmd...\n");
            if (is_node_cmd(to_check))
            {
                // printf("si tratta di un node_cmd...\n");
                // printf("node_cmd2,return: to_check\n");
                return (to_check);
            }
            else
            {
                // printf("ELSE,RETURN:go_to_starter_node(to_check))\n");
                return (go_to_starter_node(to_check));
            }
        }
        else // dovrebbe essere il caso in cui il node_back è shell->tree(la root)
        {
            // printf("next_cmd2:return NULL( ELSE )\n");
            return (NULL);
        }
           
    }
}
// 5 func