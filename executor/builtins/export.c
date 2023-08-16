/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:19:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/16 23:01:17 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// export:

// Per implementare il comando export, devi gestire la creazione e
// la modifica delle variabili d'ambiente nel tuo programma. Le variabili
//  d'ambiente sono generalmente memorizzate come coppie di chiavi e valori.
//  Puoi utilizzare le funzioni di libreria get() e setenv() per ottenere e impostare
//   le variabili d'ambiente, rispettivamente.

// Funzione per scambiare due stringhe
void swap_(char **str1, char **str2)
{
    char *temp;

    temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

// Funzione Bubble Sort per ordinare l'array di stringhe
void bubble_sort_strings(char **strings, int num_strings)
{
    int i;
    int j;

    i = -1;
    while (++i < num_strings - 1)
    {
        j = -1;
        while (++j < num_strings - i - 1)
        {
            if (ft_strcmp(strings[j], strings[j + 1]) > 0)
                swap_(&strings[j], &strings[j + 1]);
        }
    }
}

void ft_conc_2(t_shell *shell, char *str, int i)
{
    int y;
    // int j;
    char *str1;

    y = -1;
    printf("i:%d|str:%s|ft_strlen(str):%d\n", i, str, ft_strlen(str));
    // str1 = malloc(ft_strlen(str) + 1);  // OLD
    str1 = malloc(ft_strlen(str)); // DEBUG
    if (!str1)
        return (exit(1));
    while (str[++y] != '+')
        str1[y] = str[y];
    while (str[++y] != '+' && str[y - 1])
        str1[y - 1] = str[y];
    shell->env = ft_realloc(shell->env, sizeof(char *) * (i + 2));
    shell->env[i] = str1;
    printf("shell->env[i:%d]:%s\n", i, shell->env[i]);
    shell->env[i + 1] = 0; // OLD
    // free(str1);
}
void ft_conc(t_shell *shell, char *str, int y)
{
    int j;
    int i;
    int x;
    char *new_str;
    
    x = 1;
    i = -1;
    printf("in conc, y:%d\n", y);
    while (shell->env[++i])
    {
        printf("shell->env[i:%d]:%s\n",i, shell->env[i]);
        printf("ft_strncmp:%d\n", ft_strncmp(shell->env[i], str, y, 1));
        printf("\nn.elementidienv:%d\n", i);
        if ((ft_strncmp(shell->env[i], str, y, 1)) == 0)
        {
            j = -1;
            while (str[++j])
            {
                if (str[j] == '=')
                    break ;
            }
            printf("j:%d\n",j);
            // while (shell->env[i][++j])
            // {
            //     if (shell->env[i][j] == '=')
            //         x += 1;
            //     printf("y:%d|x:%d\n",y, x);
            // }
            // j = -1;
            // while (shell->env[i][++j])
            //     ;
            // printf("\n\nnon-capisco%c\n\n", str[y + 2]);
            // while (str[y + x])
            //     shell->env[i][j++] = str[y++ + x];
            // shell->env[i][j + 1] = 0;
            // creare stringa
            new_str = ft_strjoin(shell->env[i], str + ++j );
            printf("new_str:%s|shell->env[i]:%s|str + j + 1:%s\n", new_str,shell->env[i], str + j);
            free(shell->env[i]);
            shell->env[i] = new_str;
            return;
        }
    }
    printf("entra in conc_2\n");
    ft_conc_2(shell, str, i);
    printf("\n\nnon-capiscoshell:%s\n\n", shell->env[i]);
}

int ft_get_equ(char *env)
{
    int i;

    i = -1;
    while (env[++i] && env[i] != '=')
        ;
    return (i);
}

int ft_reach(t_shell *shell, char *str)
{
    int i;
    int y;

    y = -1;
    i = -1;
    while (str && str[++y] != '=')
        ;
    printf("dopo while, y:%d\n", y);
    printf("str:%s|y:%d|str[y]:%c|str[y-1]:%c\n", str, y, str[y], str[y - 1]);
    if (str && (str[y - 1] == '+'))
    {
        printf("22\n");
        ft_conc(shell, str, --y);
        printf("\n\nnon-entratoft_reach:%s\n\n", str);
        return (1);
    }
    printf("\n\nnon-uscito da ft_reach:%s\n\n", shell->env[i]);
    while (shell->env[++i])
    {
        if ((ft_strncmp(shell->env[i], str, y, 1)) == 0)
        {
            if (str[y] == '=')
            {
                free(shell->env[i]);
                shell->env[i] = str;
                return (1);
            }
            return (1);
        }
    }
    return (0);
}

void ft_name_value(t_node *node, t_shell *shell, int ij, int y)
{
    int i;
    int j;

    j = -1;
    i = 0;
    printf("Q\n");
    while (shell->env[++j])
        ;
    printf("A\n");
    while (node->content.cmd[y][++i])
        ;
    printf("B\n");
    if (ij > y)
    {
        printf("ij:%d|node->content.cmd[y]:%s\n",ij, node->content.cmd[y]);
        if ((ft_reach(shell, node->content.cmd[y]) == 1))
        {
            if (node->content.cmd[y + 1])
                ft_name_value(node, shell, ij, ++y);
            return;
        }
        shell->env = ft_realloc(shell->env, sizeof(char *) * (j + 2));
        shell->env[j] = node->content.cmd[y];
        shell->env[j + 1] = 0;
    }
    if (ij - 1 == y)
        return;
    else
        ft_name_value(node, shell, ij, ++y);
}

int ft_check(t_node *node)
{
    int i;
    int y;

    i = 1;
    printf("entra in ft_check...\n");
    while (node->content.cmd[i])
    {
        if ((node->content.cmd[i][0] >= 'a' && node->content.cmd[i][0] <= 'z') || (node->content.cmd[i][0] >= 'A' && node->content.cmd[i][0] <= 'Z'))
            ++i;
        else
            return (write(1, "checkexpo\n", 11) - 10);
    }
    printf("esce dal while ft_check...\n");
    i = 0;
    y = -1;
    while (node->content.cmd[++i])
    {
        while (node->content.cmd[i][++y])
        {
            if ((node->content.cmd[i][y] >= '0' && node->content.cmd[i][y] <= '9') || (node->content.cmd[i][y] >= 'a' && node->content.cmd[i][y] <= 'z') || (node->content.cmd[i][y] >= 'A' && node->content.cmd[i][y] <= 'Z') || node->content.cmd[i][y] == '=' || node->content.cmd[i][y] == '+')
                continue;
            else
                return (write(1, "checkexpo1\n", 11) - 10);
        }
    }
    printf("esce dal secondo while ft_check...\n");
    return (0);
}

void ft_export(t_shell *shell, t_node *node)
{
    int i;
    int y;
    int ij;
    int len;

    y = 0;
    ij = ft_get_len_mat(node);
    printf("ft_get_len_mat(node):%d\n", ft_get_len_mat(node));
    if (ij >= 2)
    {
        if (ft_check(node) == 1)
        {
            printf("esce in  ft_check(node) == 1\n");
            return;
        }
        printf("prima ft_name_value(node, shell, ij, 1);\n");
        ft_name_value(node, shell, ij, 1);
        printf("dopo ft_name_value(node, shell, ij, 1);\n");
        return;
    }
    else
    {
        while (shell->env[++y])
            ;
        i = -1;
        while (shell->env[++i])
            bubble_sort_strings(shell->env, y);
        printf("in else, dopo bubble_sort\n");
        i = -1;
        while (shell->env[++i])
        {
            y = -1;
            printf("declare -x ");
            len = ft_get_equ(shell->env[i]);
            // printf("\nlen:%d\n", len);
            while (++y <= len)
                printf("%c", shell->env[i][y]);
            if (len < (int)ft_strlen(shell->env[i]))
                printf("\"%s\"", shell->env[i] + len + 1);
            printf("\n");
            // printf("declare -x %s\n", shell->env[i]);
        }
        printf("fine else....\n");
    }
}
