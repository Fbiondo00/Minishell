/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:28 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/12 01:26:28 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_isdigit(int n)
{
   
	if ((n < 48 || n > 57))
		return (0);
	return (1);
}

int	is_number(char *str)
{
    int i;

    i = -1;
	while (str[++i])
	{
        if(str[0]=='+' || str[0]=='-')
        {
            i++;
            continue;
        }
            
		if (!ft_isdigit(str[i]))
			return (0);
	        i++;
	}
	return (1);
}

void free_all2_0(t_shell *shell, char *str, int exit_status, int to_exit)
{
    free_envp(shell);
    ft_clean_exit(shell, str, exit_status, to_exit);
}
void ft_exit_2(t_node *node, t_shell *shell, int i)
{
     int y; 
     
    if(!is_number(node->content.cmd[i -1]))
        {
            free_all2_0(shell,0,1000,0);
            exit (printf("\nexit: %s: numeric argument required\n", node->content.cmd[1]) * 0 + 255);
        }
        else
            {
                y = ft_atoi(node->content.cmd[i-1]); 
                while(y >= 256)
                    y-=256;
                while(y < 0)
                    y+= 256;
                free_all2_0(shell, "exit", y,1);   
            }
}

int ft_exit(t_node *node, t_shell *shell)
{
    int i;

    i = -1;
    while(node->content.cmd[++i])
        ;
    if(i == 1)
         free_all2_0(shell, "exit", 0,1);   
    else 
    {
        if(i == 3)
            return(write(1,"exit: too many arguments\n\n" ,25)*0);  
        ft_exit_2(node, shell, i);
    }
    return(0);  
}
