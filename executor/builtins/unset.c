/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:18 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/10 16:21:00 by flaviobiond      ###   ########.fr       */
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

int ft_check_unset (t_node *node)
{
	int i;
	int y;
	
	i = 1;
	while(node->content.cmd[i])
	{
		if((node->content.cmd[i][0] >= 'a' && node->content.cmd[i][0] <= 'z') || (node->content.cmd[i][0] >= 'A' && node->content.cmd[i][0] <= 'Z'))
				++i;
		else
		return(write(1, "checkexpo\n", 11)-10);
	}
	i = 0;
	y = -1;
	while(node->content.cmd[++i])
	{
		while(node->content.cmd[i][++y])
		{
	 		if ((node->content.cmd[i][y]  >= '0' && node->content.cmd[i][y]  <= '9') || (node->content.cmd[i][y]  >= 'a' && node->content.cmd[i][y]  <= 'z')
				|| (node->content.cmd[i][y]  >= 'A' && node->content.cmd[i][y]  <= 'Z'))
				{
				
			
				}
			else
			return(write(1, "checkexpo1\n", 11)-10);
		}
	}	
	return (0);
}

void    ft_unset(t_node *node, t_shell *shell)
{
    int ij;

    ij = ft_get_len_mat(node);
    if(!node->content.cmd[1])
        return ;
    if(ij >= 2)
    {
        if(ft_check_unset(node) == 1)
			return ;
        ft_ricorsione(node, shell, ij, 1);
    }
    
}