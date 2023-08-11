/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:28 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/11 18:18:19 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_isdigit(int n)
{
    if( n != '+' || n!= '-')
        return (1);
	if (n < 48 || n > 57)
		return (0);
	return (1);
}

int	is_number(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}


int ft_exit(t_node *node)
{
    int i;
    int y;

    i = -1;
    
    while(node->content.cmd[++i])
        ;
    if(i == 1)
    {
        write(1, "\nexit\n", 7);
        exit(0);
    }
    else 
    {
        
        if(!is_number(node->content.cmd[i -1]))
            exit (printf("exit: %s: numeric argument required\n", node->content.cmd[1]) * 0 + 255);
        else
        
            {
                y = ft_atoi(node->content.cmd[i-1]); 
                printf("y:%d", y);
                while(y >= 256)
                    y-=256;
                while(y < 0)
                    y+= 256;
                exit(y);
                
            }
        
    }
    return(0);
    
}
// ft_clean_exit()
