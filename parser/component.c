/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   component.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:49:26 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/30 16:59:23 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void ft_lowercase_cmd(t_node *node)
{
    int i;

    i = -1;
    while (node->content.cmd[0] && node->content.cmd[0][++i])
    {
        if (node->content.cmd[0][i] >= 65 && node->content.cmd[0][i] <= 90)
            node->content.cmd[0][i] += 32;
    }
}


// ritorna un int contenente la differenza tra parentesi "(" e quelle ")"
// sostituisce le parentesi con gli spazi
// fa anche il set_cmd
int calculate_lvl_diff(t_node *node)
{
    int i;
    int count;

    i = -1;
    count = 0;
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
        {
            node->raw_cmd[i] = ' ';
            count++;
        }
        if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
        {
            node->raw_cmd[i] = ' ';
            count--;
        }
    }
    set_cmd(node);
    return (count);
}


void print_node(t_shell *shell, t_node *node)
{
    int i;

    i = -1;
    if (shell && shell->tree->content.idx_op != -1)
        printf("\n\nBASH:%s\n", shell->rawline);
    if (shell && shell->tree->content.idx_op == -1)
        printf("\n\nBASH:%s\n", shell->str);
    printf("---------------------------\n");
    printf("----------|NODO|-----------\n");
    printf(">node:%p\n", node);
    while (node->content.cmd[++i])
        printf("cmd[%d]:|%s|len:%d ", i, node->content.cmd[i], ft_strlen(node->content.cmd[i]));
    printf("\n");
    printf("-----|REDIRECTION|-----\n");
    if (node->content.redir)
    {
        i = -1;
        printf("node->content.kv_size:%d\n", node->content.kv_size);
        while (++i < node->content.kv_size)
        {
            if (!i)
                printf("-----------------------\n");
            printf("REDIR N°%d\n", i + 1);
            printf("FD:%d| ", node->content.redir[i].fd);
            printf("LVL:%d| ", node->content.redir[i].lvl);
            if (node->content.redir[i].key == 13)
                printf("KEY: R_INPUT| ");
            if (node->content.redir[i].key == 14)
                printf("KEY: R_INPUT_HERE_DOC| ");
            if (node->content.redir[i].key == 15)
                printf("KEY: R_OUTPUT_TRUNC| ");
            if (node->content.redir[i].key == 16)
                printf("KEY: R_OUTPUT_APPEND| ");
            printf("VALUE:{%s}len:%d\n", node->content.redir[i].value, ft_strlen
            (node->content.redir[i].value));
        }
    }
    if (!node->content.redir)
        printf("il nodo non ha redirezioni!\n");
    if (node->back)
    {
        printf("-------|back node|-------\n");
        printf("indirizzo:%p\n", node->back);
        if (node->back->content.op == 10)
        printf("operatore: AND\n");
        if (node->back->content.op == 11)
        printf("operatore: OR\n");
        if (node->back->content.op == 12)
        printf("operatore: PIPE\n");
        printf("lvl_subshell:%d\n", node->back->lvl_subshell);
        if (node->back->back)
        {
            printf("----|back>back node|----\n");
            printf("indirizzo:%p\n", node->back->back);
            if (node->back->back->content.op == 10)
                printf("operatore: AND\n");
            if (node->back->back->content.op == 11)
                printf("operatore: OR\n");
            if (node->back->back->content.op == 12)
                printf("operatore: PIPE\n");
            printf("lvl_subshell:%d\n", node->back->back->lvl_subshell);
        }
        if (!node->back->back)
            printf("node->back->back:(null)!!\n");
    }
    if (!node->back)
        printf("node->back:(null)!!\n");
    printf("------|==FINE_NODO==|------\n");
    printf("---------------------------\n");
}

// V2 con next_cmd2
//  naviga l albero e:
//  se trova nodo_cmd setta exp e fa le varie rimozioni e setta lvl sui corretti nodi op
//  se trova nodo_op continua a navigare l albero
void set_components(t_shell *shell)
{
    t_node *node;
    int lvl_diff;

    printf("------------------|FASE: SET COMPONENTS ALBERO|------------------\n");
    // CASO BASE: ASSENZA DI NODO_OP
    // in questo caso ignora il valore di ritorno di calculate_lvl_diff
    // perche essendo un solo comando non si esegue la subshell
    if (is_node_cmd(shell->tree))
    {
        printf("ESCE:is_node_cmd(shell->tree)\n");
        calculate_lvl_diff(shell->tree);
        // ......PRINT ............
        print_node(shell, shell->tree);
        return;
    }
    // CASO : PRESENZA ALMENO 1 NODO_OP
    // IL PRIMO STARTER NODE VIENE DAL LEFT-BRANCH
    node = go_to_starter_node(shell->tree->left);
    // ci sono DUE LOOP, il primo gestisce tutti i branch e sub-branch
    // del LEFT, il secondo gestisce quelli del RIGHT
    // CARATTERISTICHE PRIMO LOOP:
    // esce quando non esiste piu il back node
    // se diff>0 in right branch cè sempre il back del back node
    printf("INIZIO LOOP...\n");
    while (1)
    {
        // printf("PRE:calculate_lvl_diff(node)\n");
        lvl_diff = calculate_lvl_diff(node);
        // printf("POST:calculate_lvl_diff(node)\n");
        if (is_left_branch(node))
        {
            // printf("is_left_branch(node)\n");
            node->back->lvl_lock = 1;
            node->back->lvl_subshell += lvl_diff;
        }
        if (next_cmd2(shell, node) && !is_left_branch(node) && !node->back->back->lvl_lock)
        {
            // printf("!is_left_branch(node) && !node->back->back->lvl_lock\n");
            // printf("PRIMO LOOP:NON LEFT BRANCH && !LOCK\n");
            node->back->back->lvl_lock = 1;
            node->back->back->lvl_subshell += (lvl_diff + node->back->lvl_subshell);
        }
        else if (next_cmd2(shell, node) && !is_left_branch(node) && node->back->back->lvl_lock)
        {
            // printf("!is_left_branch(node) && node->back->back->lvl_lock\n");
            // printf("PRIMO LOOP:NON LEFT BRANCH && LOCK\n");
            node->back->back->lvl_lock = 1;
            node->back->back->back->lvl_subshell += (lvl_diff + node->back->lvl_subshell);
        }
        // ......PRINT LVL............
        // printf("calculate_lvl_diff(node):%d\n", lvl_diff);
        print_node(shell, node);
        // ...........................
        node = next_cmd2(shell, node);
        if (!node) // è NULL se è stato navigato tutto il left branch
            break;
    }
    printf("FINE LOOP\n");
}
