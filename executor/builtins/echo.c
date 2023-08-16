/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:20:11 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/16 05:15:29 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// echo:

// Per implementare il comando echo, devi semplicemente stampare
//  l'output sulla console per ogni argomento passato al comando echo.

int is_valid(t_node *node)
{
    int i;

    if (node->content.cmd[1] == NULL)
        return (0);
    if (node->content.cmd[1][0] != '-')
        return (0);
    i = 1;
    while (i < ft_strlen(node->content.cmd[1]))
    {
        if (node->content.cmd[1][i++] != 'n')
            return (0);
    }
    return (1);
}

void ft_echo(t_node *node)
{
    int i;
    int flot;
    int y;

    flot = 0;
    i = 0;
    y = 0;

    while (node->content.cmd[y++])
        ;
    flot = is_valid(node);
    if (flot == 1)
        i++;
    while (node->content.cmd[++i])
    {
        printf("%s", node->content.cmd[i]);
        if (i < y - 2)
            printf(" ");
    }
    if (flot == 0)
        printf("\n");
}
