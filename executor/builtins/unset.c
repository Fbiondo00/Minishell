/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:18 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/09 18:00:46 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void ft_ricorsione(t_node *node, t_shell *shell, int ij, int y)
{
    int i;
    char **str;
    

    i = -1;
    str = shell->env;
    
    while(shell->env[++i])
    {
        if((ft_strncmp(shell->env[i], node->content.cmd[y], ft_strlen(node->content.cmd[y])))==0)
        {
            printf("\n%s\n", shell->env[i]);
            free(shell->env[i]);
            
        }
    }
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