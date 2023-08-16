/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:55:33 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/16 19:32:51 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
        if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34 || node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$')
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
    // printf("TOT_LEN:%d\n", tot_len);
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
    printf("IN var_expand\n");
    while (node->shell->env[++i])
    {
        // printf("i:%d\n", i);
        // ritorna 0 se sono uguali
        if (!ft_strncmp2(str, node->shell->env[i], get_idx_eq(node, i), 1))
        {
            new_str = node->shell->env[i] + get_idx_eq(node, i) + 1;
            printf("get_idx_eq:%d\n", get_idx_eq(node, i));
            printf("node->shell->env[i]:%s|\n", node->shell->env[i]);
            printf("new_str:%s|\n", new_str);
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
    printf("y:%d| strlen raw_cmd:%d\n", y, ft_strlen(node->raw_cmd));
    while (y < ft_strlen(node->raw_cmd) && node->raw_cmd[y] != ' ' && node->raw_cmd[y] != 34 && node->raw_cmd[y] != 39 && node->raw_cmd[y] != '$')
    {
        y++;
        flag++;
    }
    printf("POST y:%d| FLAG:%d|node->raw_cmd[y]:%c\n", y, flag, node->raw_cmd[y]);
    if (flag == 1)
        return (NULL);
    str = malloc(flag + 1);
    str[flag] = '\0';
    i = -1;
    while (flag-- > 0)
        str[++i] = node->raw_cmd[++idx];
    printf("VARIABILE:%s\n", str);
    return (var_expand(node, str));
}

// fa le espansioni.
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
            // printf("IN node->raw_cmd[i] == '$' \n");
            // printf("in_quotes(node, i):%d\n", in_quotes(node, i));
            // ritorna il valore della variabile
            str = find_var(node, i);
            // printf("str:%s\n", str);
            // if (str)
            // {
            raw = modify_raw_and_quote(node, i, str, 32);
            // free(node->raw_cmd);
            node->raw_cmd = raw;
            quote = modify_raw_and_quote(node, i, str, 48);
            // free(node->quote_idx);
            node->quote_idx = quote;
            // free(str); --> ?
            // printf("NEW RAW_CMD|%s|strlen:%d\n", node->raw_cmd, ft_strlen(node->raw_cmd));
            // printf("NEW QUOTE_IDX|%s|strlen:%d\n", node->quote_idx, ft_strlen(node->quote_idx));
            // }
        }
    }
}

// elabora la stringa e produce l array di stringhe
// da dare in pasto all EXECVE
// tuttavia deve ignorare le redirection che potrebbero essere d intralcio
// in quanto possono capitare in mezzo al comando.
// probabile questo problema debba esser risolto in set_redirection
// che rielabora nuovamente la raw_cmd andando a fare il trim dei char che
// ha usato per la redirection
// NUOVO: ho aggiunto attributo shell al nodo
void set_cmd(t_node *node)
{
    // creare file.c contenente questo set di funzioni
    // set_lvl(node);
    ft_do_expand(node);
    // printf("--- AFTER EXPAND ---\n");
    // printf("NEW RAW_CMD|%s|strlen:%d    ", node->raw_cmd, ft_strlen(node->raw_cmd));
    // printf("NEW QUOTE_IDX|%s|strlen:%d\n", node->quote_idx, ft_strlen(node->quote_idx));
    remove_quotes(node);
    // printf("--- AFTER REMOVE_QUOTES ---\n");
    // printf("NEW RAW_CMD|%s|strlen:%d    ", node->raw_cmd, ft_strlen(node->raw_cmd));
    // printf("NEW QUOTE_IDX|%s|strlen:%d\n", node->quote_idx, ft_strlen(node->quote_idx));
    node->content.cmd = ft_split(node->raw_cmd, ' ');
    ft_lowercase_cmd(node); // solo cmd[0]
    // printf("---  LOWERCASE_CMD[0] ---\n");
    // while (node->content.cmd[++i])
    //     printf("cmd[%d]:%s ", i, node->content.cmd[i]);
    // printf("\n");
}

// 5 func