/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mod_raw_a_qu.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 22:19:31 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/30 23:01:42 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// $?x
char *modify_raw_and_quote3(t_node *node, int idx, char *str, char c)
{
    int i;
    int j;
    int y;
    int tot_len;
    char *new_st;

    y = idx + 2;
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
    else if (c == 48)
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
    while (y < ft_strlen(node->raw_cmd))
    {
        y++;
        if (node->raw_cmd[y] == ' ' || node->raw_cmd[y] == 34 || node->raw_cmd[y] == 39 || node->raw_cmd[y] == '$' || node->raw_cmd[y] == '/')
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
    else if (c == 48)
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