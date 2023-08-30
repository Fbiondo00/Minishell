/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:55:33 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/30 16:58:54 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// idx of $
char *modify_raw_and_quote2(t_node *node, int idx, char *str, char c)
{
    int i;
    int j;
    int y;
    int tot_len;
    char *new_st;

    y = idx + 1;
    while (y < ft_strlen(node->raw_cmd))
    {
        y++;
        if (node->raw_cmd[y] == '$')
        {
            if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34 || node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$' || node->raw_cmd[y] == '/')
                break;
        }
        else if (node->raw_cmd[y] == ' ')
            break;
    }
    if (str)
        tot_len = idx + ft_strlen(str) + ft_strlen(node->raw_cmd) - y;
    if (!str)
        tot_len = ft_strlen(node->raw_cmd);
    new_st = malloc(tot_len + 1);
    new_st[tot_len] = '\0';
    i = -1;
    j = -1;
    if (c == 32)
    {
        while (++i < tot_len)
        {
            if (i < idx)
                new_st[i] = node->raw_cmd[i];
            else if (!str && (i >= idx && i < y))
                new_st[i] = ' ';
            else if (!str && !(i >= idx && i < y))
                new_st[i] = node->raw_cmd[y++];
            else if (str && ++j < ft_strlen(str))
                new_st[i] = str[j];
            else if (str && j >= ft_strlen(str))
            {
                new_st[i] = node->raw_cmd[y++];
                j++;
            }
        }
    }
    else if (c == 48) // ovvero SPACE, quote_idx
    {
        while (++i < tot_len)
        {
            if (node->raw_cmd[i] == 34)
                new_st[i] = 34;
            else if (node->raw_cmd[i] == 39)
                new_st[i] = 39;
            else
                new_st[i] = 48;
        }
    }
    return (new_st);
}

// idx of $
char *modify_raw_and_quote(t_node *node, int idx, char *str, char c)
{
    int i;
    int j;
    int y;
    int tot_len;
    char *new_st;

    y = idx + 1;
    // printf("PRE:Y:%d\n", y);
    while (y < ft_strlen(node->raw_cmd))
    {
        y++;
        if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34 || node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$' || node->raw_cmd[y] == '/')
            break;
    }
    // printf("POST:Y:%d\n", y);
    // TOT_LEN = idx($) +  ft_strlen(str) + strlen-idx(fine$) + 1;
    if (str)
        tot_len = idx + ft_strlen(str) + ft_strlen(node->raw_cmd) - y; // LEN SE VAR EXISTE
    if (!str)
        tot_len = ft_strlen(node->raw_cmd);
    //  tot_len = ft_strlen(node->raw_cmd) - (y - idx); // LEN SE VAR NON EXISTE
    // printf("ft_strlen(node->raw_cmd):%d|y:%d|idx:%d\n", ft_strlen(node->raw_cmd), y, idx);
    printf("TOT_LEN:%d\n", tot_len);
    // printf("ft_strlen(node->raw_cmd) - y:%d|y:%d\n", ft_strlen(node->raw_cmd) - y, y);
    new_st = malloc(tot_len + 1);
    new_st[tot_len] = '\0';
    i = -1;
    j = -1;
    if (c == 32) // ovvero SPACE, raw_cmd
    {
        // printf("MODIFICO RAW_CMD\n");
        while (++i < tot_len)
        {
            // printf("i:%d|IDX:%d\n", i, idx);
            if (i < idx)
            {
                // printf("A\n");
                new_st[i] = node->raw_cmd[i];
            }
            else if (!str && (i >= idx && i < y))
            {
                new_st[i] = ' ';
                // c$HOMEc$HOME
            }
            else if (!str && !(i >= idx && i < y))
            {
                new_st[i] = node->raw_cmd[y++];
            }
            else if (str && ++j < ft_strlen(str))
            {
                // printf("B|j:%d|ft_strlen(str):%d\n", j, ft_strlen(str));
                new_st[i] = str[j];
            }
            else if (str && j >= ft_strlen(str))
            {
                // printf("C\n");
                new_st[i] = node->raw_cmd[y++];
                j++;
            }
            // printf("D\n");
        }
    }
    else if (c == 48) // ovvero SPACE, quote_idx
    {
        // printf("MODIFICO QUOTE_IDX\n");
        // printf("VALORE ATTUALE:%s\n", node->quote_idx);

        while (++i < tot_len)
        {
            if (node->raw_cmd[i] == 34)
                new_st[i] = 34;
            else if (node->raw_cmd[i] == 39)
                new_st[i] = 39;
            else
                new_st[i] = 48;
        }
    }
    return (new_st);
}

// fa il free di str
// ritorna NULL se non trova variabile o se variabile senza =
// ritorna nuova str contenente il valore
// il char ritornato NON e'da friare
char *var_expand(t_node *node, char *str)
{
    int i;
    char *new_str;

    i = -1;
    new_str = NULL;
    while (node->shell->env[++i])
    {
        if (!ft_strncmp2(str, node->shell->env[i], get_idx_eq(node, i), 1))
        {
            new_str = node->shell->env[i] + get_idx_eq(node, i) + 1;
            break;
        }
    }
    if (new_str)
        printf("value of%s is %s\n", node->shell->env[i], new_str);
    if (!new_str)
        printf("NEW_STR e' NULL\n");
    free(str);
    return (new_str);
}

// idx corrisponde a index di $
// ritorna la stringa della variabile
// ritorna null se non c e' alcun char attaccato a seguire
char *find_var(t_node *node, int idx)
{
    int i;
    int y;
    int flag;
    char *str;

    flag = 0;
    y = idx + 1;
    while (y < ft_strlen(node->raw_cmd) && node->raw_cmd[y] != ' ' && node->raw_cmd[y] != 34 && node->raw_cmd[y] != 39 && node->raw_cmd[y] != '$' && node->raw_cmd[y] != '/')
    {
        y++;
        flag++;
    }
    if (flag == 0)
        return (NULL);
    str = malloc(flag + 1);
    str[flag] = '\0';
    i = -1;
    while (flag-- > 0)
        str[++i] = node->raw_cmd[++idx];
    return (var_expand(node, str));
}

// espande le variabili.
// se in single quote non espande
// se in double quote espande
// piccolo problema: se fai espansione poi non coincide quote_idx con idx del raw_cmd
// soluzione, aggiungere spazi in quote idx e realloc
// $HOME cc /$HOME"  / "$HOME" BUG
// $HOME"ciao" > bash: /Users/rdolziciao: No such file or directory
// SE METTO SOLO 1 " O ' errore free, dopo un primo comando, se lo si fa subito da syntax error.
// "$HOME"  poi "$HOME
// $HOME$HOME  ->COME STOP DELLA PAROLA DA PRENDERE + COME STOP PER MALLOC NEW_STR
void ft_do_expand(t_node *node)
{
    int i;
    char *raw;
    char *quote;
    char *str;
    i = -1;
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == '$' && in_quotes(node, i) != -1)
        {
            str = find_var(node, i);
            raw = modify_raw_and_quote(node, i, str, 32);
            // free(node->raw_cmd);
            node->raw_cmd = raw;
            quote = modify_raw_and_quote(node, i, str, 48);
            // free(node->quote_idx);
            node->quote_idx = quote;
        }
    }
}

void ft_do_asterisk(t_node *node)
{
    int i;
    int y;

    i = -1;
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == '*' && !in_quotes(node, i))
        {
                y = i;
                while (node->raw_cmd[y])
                {
                if (node->raw_cmd[y] == ' ')
                    break;
                y--;
                }
                y++;
                ft_wild(node, i, y);
        }
    }
}


void set_cmd(t_node *node)
{
    ft_do_asterisk(node);
    ft_do_expand(node);
    ft_do_cmd(node);
    ft_lowercase_cmd(node);
}

// 5 func
