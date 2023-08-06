/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:20:11 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/06 17:08:52 by flaviobiond      ###   ########.fr       */
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
    int ij;

    flot = 0;
    i = 0;
    y = 0;
    ij= 0;

    while (node->content.cmd[y++])
        ;
    flot = is_valid(node);
    if (flot == 1)
        i++;
    if(strcmp(node->content.cmd[ij++], "$"))
        printf("\n%s\n",node->content.cmd[ij] );
    while (node->content.cmd[++i])
    {
        printf("%s", node->content.cmd[i]);
        if (i < y - 2)
            printf(" ");
    }
    if (flot == 0)
        printf("\n");
}
