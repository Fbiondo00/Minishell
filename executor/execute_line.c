/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/09 17:40:26 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// TODO:
// Per debug
//     -creare funzione che stampa il nodo ed i suoi attributi
//     -creare funzione che naviga i nodi
//     -creare funzione che accede al nodo di start execution, ovvero
//         all ultimo nodo riscontrato fra tutte le left_branchç


void print_node(t_node *node)
{
    int i;

    i = -1;
    if (node->content.op)
        printf("----NODE OPERATOR----\n");
    else if (!node->content.op)
        printf("----NODE CMD----\n");
    printf("---attributes---\n");
    printf("Node address: %p\n", node);
    printf("raw_cmd: %s\n", node->raw_cmd);
    printf("back_node: %p\n", node->back);
    printf("left_node: %p\n", node->left);
    printf("rigth_node: %p\n", node->right);
    printf("---NODE CONTENT---\n");
    if (node->content.op == AND)
    printf("operator: AND\n");
    if (node->content.op == OR)
    printf("operator: OR\n");
    if (node->content.op == PIPE)
    printf("operator: PIPE\n");
    printf("kv_size: %d\n", node->content.kv_size);
    printf("---LIST OF KEY/VALUE REDIR---\n");
    while (++i < node->content.kv_size)
    {
        printf("--REDIR n°%d--\n",i);
        if (node->content.redir->key == R_INPUT)
            printf("KEY: R_INPUT\n");
        if (node->content.redir->key == R_INPUT_HERE_DOC)
            printf("KEY: R_INPUT_HERE_DOC\n");
        if (node->content.redir->key == R_OUTPUT_TRUNC)
            printf("KEY: R_OUTPUT_TRUNC\n");
        if (node->content.redir->key == R_OUTPUT_APPEND)
            printf("KEY: R_OUTPUT_APPEND\n");
    }
    printf("VALUE: %s\n", node->content.redir->value);
        printf("----------------------\n");
}
