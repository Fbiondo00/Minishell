/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:18 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/09 23:41:38 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void ft_ricorsione(t_node *node, t_shell *shell, int ij, int y)
{
    int i;
    int j;

    i = -1;
    
    while(shell->env[++i])
    {
    j = -1;
        while( shell->env[i][++j] != '=' && shell->env[i][j])
        ;
        if(((ft_strncmp(shell->env[i], node->content.cmd[y], j , 0)) == 0))
        {
             if(((ft_strncmp(shell->env[i], node->content.cmd[y], ft_strlen(node->content.cmd[y]) , 0)) == 0))
            {
            printf("\n%d\n, ", j);
            printf("IN STRN%s\n", shell->env[i]);
            free(shell->env[i]);
            break ;
            }
        }
        
    }
    while (shell->env[i + 1])
    {
        printf("i:%d", i);
        swap_(&shell->env[i], &shell->env[i + 1]);
        i++;
    }
    shell->env[i] = 0;
    if(node->content.cmd[y + 1])
         ft_ricorsione(node, shell, ij, ++y);
    else
    return ;
}

void    ft_unset(t_node *node, t_shell *shell)
{
    int ij;

    ij = ft_get_len_mat(node);
    if(!node->content.cmd[1])
        return ;
    if(ij >= 2)
        ft_ricorsione(node, shell, ij, 1);
    
}