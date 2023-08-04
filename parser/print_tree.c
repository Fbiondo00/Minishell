/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tree.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 00:10:47 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/21 16:51:20 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
static int node_16[12] = {0, 1, 5, 14, 17, 38, 41, 38, 65, 86, 113, 113}; 
static int node_16_rev[12] = {113, 113, 65, 86, 41, 38, 10, 15, 5, 14, 0, 1}; // {113, 113, 65, 86, 41, 38, 17, 38, 5, 14, 0, 1}; quello studiato in origine
static int node_8[12] = {0, 2, 6, 14, 18, 38, 41, 41, 0, 0, 0, 0};
static int node_8_rev[12] = {41, 41, 18, 38, 6, 14, 0, 2, 0, 0, 0, 0};
static int node_4[12] = {0, 4, 7, 18, 21, 21, 0, 0, 0, 0, 0, 0};
static int node_4_rev[12] = {21, 21, 7, 18, 0, 4, 0, 0, 0, 0, 0, 0};
static int node_2[12] = {0, 8, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0};
static int node_2_rev[12] = {9, 9, 0, 8 , 0, 0, 0, 0, 0, 0, 0, 0};
static int node_1_rev[12] = {10, 10 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// ritorna il numero di livelli presenti.
int    get_levels(t_shell *shell)
{
    int count;
    t_node *nod;

    count = 0;
    if (shell->tree)
    {
        nod = shell->tree;
        while (nod)
        {
            count++;
            nod = nod->left;
        }
    }
    return (count);
}

// sono i nodi totali che ci potrebbero essere se tutti i rami fossero completamente
// sviluppati. altrimenti sono di meno.
int nodes_in_lvl(int lvl)
{
    int i;
    int j;
    int res;

    j = -1;
    res = 1;
    if (lvl == 0)
        return (res);
    while (++j < lvl)
        res *= 2;
    return (res);
}

void    print_space(int space)
{
    int i;

    i = -1;
    while (++i < space)
        printf(" ");
}

// TODO:
// space dinamico che in base alla lunghezza dello stampato fa la sottrazione per mantenere il margine uguale
void print_H(int space)
{
    printf("---NODE---");
    print_space(space); 
}

void print_ADDR(t_node *node, int space)
{
    int len;

    // printf("ADDR:%p",node);
    len = printf("A:%.8lx",(uintptr_t)&node%100000000);
    if (len < 10)
        print_space(10 - len);
    print_space(space);
}

void print_LEFT(t_node *node, int space)
{
    int len;
    len = printf("L:%.8lx",(uintptr_t)&node->left%100000000);
    if (len < 10)
        print_space(10 - len);
    print_space(space);
}

void print_RIGHT(t_node *node, int space)
{
    int len;

    len = printf("R:%.8lx",(uintptr_t)&node->right%100000000);
    if (len < 10)
        print_space(10 - len);
    print_space(space);
}

void print_CMD(t_node *node, int space)
{
    int len;

    len = printf("CMD:%s",node->raw_cmd);
    if (len < 10)
        print_space(10 - len);
    print_space(space);
}

// set default a 0..
void print_N_REDIR(t_node *node, int nbr, int space)
{
    printf("--REDIR!--");
    if( nbr + 1 > node->content.kv_size)
        printf("--(null)--");
    else
        printf("K:%d|V:%s", node->content.redir[nbr].key, node->content.redir[nbr].value);
    print_space(space);
}

void print_F(int space)
{
    printf("----------");
    print_space(space);
}

// in assenza del nod
void print_empty(void)
{
    printf("          ");
}

                                            
// considerata in media la lunghezza del nodo di 10
// int *get_array_space(int max_nodes)
// {
//     // start, middle, start, middle ....
//     // ci interessano gli array rev, che iniziano dal nodo base
//     static int node_16[12] = {0, 1, 5, 14, 17, 38, 41, 38, 65, 86, 113, 113};
//     int node_16_rev[12] = {113, 113, 65, 86, 41, 38, 17, 38, 5, 14, 0, 1};
//     int node_8[8] = {0, 2, 6, 14, 18, 38, 41, 41};
//     int node_8_rev[8] = {41, 41, 18, 38, 6, 14, 0, 2};
//     int node_4[6] = {0, 4, 7, 18, 21, 21};
//     int node_4_rev[6] = {21, 21, 7, 18, 0, 4};
//     int node_2[4] = {0, 8, 9, 9};
//     int node_2_rev[4] = {9, 9, 0, 8};
//     int node_1_rev[2] = {10, 10};

//     if (max_nodes == 16)
//         return (&node_16_rev);
//     if (max_nodes == 8)
//         return (node_8_rev);
//     if (max_nodes == 4)
//         return (node_4_rev);
//     if (max_nodes == 2)
//         return (node_2_rev);
//     if (max_nodes == 1)
//         return (node_1_rev);
//     return (NULL);
// }

// ritorna un array contenente una serie di nodi.
 // lvl sono il numero di livelli totali
t_node **get_array_of_node(t_shell *shell, int lvl)
{
    int i;
    int tot_nodes;
    t_node **array;
    
    i = -1;
    tot_nodes = nodes_in_lvl(lvl);
    array = malloc(sizeof(t_node *) * tot_nodes);
    array[0] = shell->tree;
    if (lvl == 1)
        return (array);
    else if (lvl == 2)
    {
        array[1] = array[0]->left;
        array[2] = array[0]->right;
    }
    else if (lvl == 3)
    {
        array[1] = array[0]->left;
        array[2] = array[0]->right;
        array[3] = array[1]->left;
        array[4] = array[1]->right;
        array[5] = array[2]->left;
        array[6] = array[2]->right;
    }
    else if (lvl == 4)
    {
        array[1] = array[0]->left;
        array[2] = array[0]->right;
        array[3] = array[1]->left;
        array[4] = array[1]->right;
        array[5] = array[2]->left;
        array[6] = array[2]->right;
        array[7] = array[3]->left;
        array[8] = array[3]->right;
        array[9] = array[4]->left;
        array[10] = array[4]->right;
        array[11] = array[5]->left;
        array[12] = array[5]->right;
        array[13] = array[6]->left;
        array[14] = array[6]->right;
    }
    else if (lvl == 5)
    {
        array[1] = array[0]->left;
        array[2] = array[0]->right;
        array[3] = array[1]->left;
        array[4] = array[1]->right;
        array[5] = array[2]->left;
        array[6] = array[2]->right;
        array[7] = array[3]->left;
        array[8] = array[3]->right;
        array[9] = array[4]->left;
        array[10] = array[4]->right;
        array[11] = array[5]->left;
        array[12] = array[5]->right;
        array[13] = array[6]->left;
        array[14] = array[6]->right;
        array[15] = array[7]->right;
        array[16] = array[7]->left;
        array[17] = array[8]->right;
        array[18] = array[8]->left;
        array[19] = array[9]->right;
        array[20] = array[9]->right;
        array[21] = array[10]->left;
        array[22] = array[10]->right;
        array[23] = array[11]->left;
        array[24] = array[11]->right;
        array[25] = array[12]->right;
        array[26] = array[12]->right;
        array[27] = array[13]->left;
        array[28] = array[13]->right;
        array[29] = array[14]->left;
        array[30] = array[14]->right;
    }
    return (array);
}
// printa tutti i nodi di un livello, print_empty in caso manca il ramo
// ottimizzare con funzione che prende come input un altra funzione, in questo caso il print_ ...
void print_lvl(t_shell *shell, int lvl, int start_space, int middle_space, int len)
{
    int i;
    int k;
    int j;
    int idx_start; // saltare tutti i nodi precedenti e prende da quello che ci interessa(salta lvl precedenti)
    int redir;
    int tot_nodes;
    t_node **nodes;

    i = 0;
    idx_start = 0;
    while (i++ <= lvl) // salta tutti gli idx dei nodi da non printare
        idx_start += nodes_in_lvl(i) - 1;
    // printf("idx_start:%d\n", idx_start);
    redir = 3;
    tot_nodes = nodes_in_lvl(lvl);
    // printf("tot_nodes:%d\n", tot_nodes);
    nodes = get_array_of_node(shell, lvl);
    printf(">>>:%s\n",nodes[0]->raw_cmd);
    // exit(0);
    i = -1;
    k = middle_space;
    while (++i < 1)
    {
        printf("LVL:%d\n", lvl);
        // printf("middle_space:%d\n", middle_space);
        // printf("start_space:%d\n", start_space);
        j = -1;
        while (++j < tot_nodes)
        {
            // if (j == tot_nodes - 1 && k >= 30)
            //     k = 20;
            // printf(">");
            if (j == 0)
                print_space(start_space);
            print_H(k);
            // printf("@");
        }
        j = -1;
        printf("\n");
        // printf("@");
        k = middle_space;
        while (++j < tot_nodes)
        {
            if (j == 0)
                print_space(start_space);
            print_ADDR(nodes[j], middle_space);
        }
        j = -1;
        printf("\n");
        while (++j < tot_nodes)
        {
            // printf("idx_start + j: %d\n", idx_start + j);
            // printf("%s\n", nodes[idx_start + j]->raw_cmd);
            // printf("tot_nodes:%d| j:%d\n", tot_nodes, j);
            if (j == 0)
                print_space(start_space);
            print_LEFT(nodes[j], middle_space); //PROBLEMA!la j fa resettare sempre all elemento 0 dell array. invece serve un contatore che scala
        }
        j = -1;
        printf("\n");
        while (++j < tot_nodes) 
        {
            if (j == 0)
                print_space(start_space);
            print_RIGHT(nodes[j], middle_space);
        }
        j = -1;
        printf("\n");
        while (++j < tot_nodes)
        {
            if (j == 0)
                print_space(start_space);
            print_CMD(nodes[j], middle_space);
        }
        printf("\n");
        // while (redir-- > 0)
        // {
        //     j = -1;
        //     while (++j < tot_nodes)
        //     {
        //         print_space(start_space);
        //         print_N_REDIR(nodes[idx_start + j], j, middle_space);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
        // printf("middle_space:%d\n", middle_space);
        // printf("start_space:%d\n", start_space);
        j = -1;
        while (++j < tot_nodes)
        {
            // if (j == tot_nodes - 1 && k >= 40)
            //     k = 40;
            // printf(">");
            if (j == 0)
                print_space(start_space);
            print_F(k);
            // printf("@");
        }
        printf("\n");
        // printf("@");
    }
    free(nodes);
}

//         <---10--->           
//<---10--->        <---10--->
// settato a un max di 3 ridirezioni
void    tree_of_life(t_shell *shell)
{
    int i;
    int j;
    int lvl;
    int nbr_nodes;
    // int *space;
    int nbr_redir = 3;
    int *space;
    
    // lo spazio e' variabile e varia in base al livello in cui ci si trova
    lvl = get_levels(shell); // sono il numero di livelli
    nbr_nodes = nodes_in_lvl(lvl);
    if (nbr_nodes == 16)
        space = node_16_rev;
    if (nbr_nodes == 8)
        space = node_8_rev;
    if (nbr_nodes == 4)
        space = node_4_rev;
    if (nbr_nodes == 2)
        space = node_2_rev;
    if (nbr_nodes == 1)
        space = node_1_rev;

    // space = get_array_space(nbr_nodes);

    // printf("LVL:%d\n", lvl);
    // printf("TOT NODES (LAST LVL):%d\n", nbr_nodes);

    i = -1;
    j = 0;
    while (++i < lvl)
    {
        // printf("START printing lvl #%d ...\n", i);
        printf("start_space:%d | middle_space:%d\n", space[j], space[j + 1]);
        print_lvl(shell, i, space[j], space[j + 1], 6 + nbr_redir);
        j+=2;
        // printf("END printing lvl #%d ...\n", i);
        printf("\n");
    }
}

*/
