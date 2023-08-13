/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:19:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/13 23:36:37 by flaviobiond      ###   ########.fr       */
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
void	swap_(char **str1, char **str2)
{
	char	*temp;

	temp = *str1;
	*str1 = *str2;
	*str2 = temp;
}

// Funzione Bubble Sort per ordinare l'array di stringhe
void	bubble_sort_strings(char **strings, int num_strings)
{
	int i;
	int j;
	
	i= -1;
	while(++i < num_strings - 1)
	{
		j=-1;
		while (++j < num_strings - i - 1)
		{
			if (ft_strcmp(strings[j], strings[j + 1]) > 0)
				swap_(&strings[j], &strings[j + 1]);
		}
	}
}

void	ft_conc_2(t_shell *shell, char *str, int i)
{
	int y;
	//int j;
	char *str1;
	
	y = -1;
	str1= malloc(ft_strlen(str));
	if(!str1)
		return(exit(1));
	//j=0;
		// printf("\n\nnon-capisco%d\n\n",i) ;
		// printf("\n\n%2s\n\n ", shell->env[i]) ;	
		 while(str[++y] != '+')
		 	str1[y]=str[y];
			// printf("\n\nnon-capisco12%s\n\n",str1) ;	
		while(str[++y] != '+' && str[y- 1])
		{
			str1[y - 1]=str[y];
		}
		shell->env = ft_realloc(shell->env, sizeof(char *) * (i + 2));
		 printf("\n\nnon-capisco11%s\n\n",str1) ;	
		shell->env[i] = str1;
			printf("\n\nnon-capiscoshell:%s\n\n",shell->env[i]) ;	
		free(str1);
        shell->env[i +1] = 0;
	
}
void ft_conc(t_shell *shell,char *str, int y)
{
	int j;
	int i;
	int x;
	
	x = 1;
	i = -1;
	j = -1;
	while(shell->env[++i])
        {
			printf("\nn.elementidienv:%d\n", i);
            if((ft_strncmp(shell->env[i], str, y, 0)) == 0)
			{
				while(shell->env[i][++j])
					{
						if(shell->env[i][j] == '=')
							x+=1;
				printf("\ny:%d\n", x);
					}
				j = -1;
				while(shell->env[i][++j])
					;
				printf("\n\nnon-capisco%c\n\n",str[y +2]) ;	
				while(str[y + x])
				shell->env[i][j++] = str[y++ + x];
				shell->env[i][j + 1] = 0;
				return ;
			}
		}
		ft_conc_2(shell, str, i);
}


int    ft_reach(t_shell *shell, char *str)
{
    int i;
    int y;

    y = -1;
    i = -1;
    while(str[++y] != '=' && str[y])
        ;
	if(str[y - 1] == '+')
	{
		ft_conc(shell,str,--y);
		return(1);
	}
    while(shell->env[++i])
        {
            if((ft_strncmp(shell->env[i], str, y, 1)) == 0)
            {
                if(str[y] == '=')
                {
                    free(shell->env[i]);
                    shell->env[i] = str;
                    return(1);
                }
                return(1);
             //printf("str:%s|env[i]:%s\n%d\n", str, shell->env[i], y);   
            }
        }
    return (0);
}

void	ft_name_value(t_node *node, t_shell *shell, int ij, int y)
{
	int	i;
	int	j;

	j = -1;
	i = 0;
	while (shell->env[++j])
		;
	while (node->content.cmd[y][++i])
        ;
	if (ij > y)
	{
        if((ft_reach(shell, node->content.cmd[y]) == 1))
        {
            if (node->content.cmd[y + 1])
                ft_name_value(node, shell, ij, ++y);
            return ;
        }
		shell->env = ft_realloc(shell->env, sizeof(char *) * (j + 2));
		shell->env[j] = node->content.cmd[y];
        shell->env[j + 1] = 0;
	}
	if (ij - 1 == y)
		return ;
	else
		ft_name_value(node, shell, ij, ++y);
}

int ft_check (t_node *node)
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
				|| (node->content.cmd[i][y]  >= 'A' && node->content.cmd[i][y]  <= 'Z') || node->content.cmd[i][y] == '=' || node->content.cmd[i][y] == '+')
						continue;
			else
			return(write(1, "checkexpo1\n", 11)-10);
		}
	}	
	return (0);
}



void	ft_export(t_shell *shell, t_node *node)
{
	int	i;
	int	y;
	int	ij;

	y = 0;
	ij = ft_get_len_mat(node);
	if (ij >= 2)
	{
		if(ft_check(node) == 1)
			return ;
		ft_name_value(node, shell, ij, 1);
		return ;
	}
	else
	{
		while (shell->env[++y])
			;
		i = -1;
		while (shell->env[++i])
			bubble_sort_strings(shell->env, y);
		i = -1;
		while (shell->env[++i])
			printf("declare -x %s\n", shell->env[i]);
	}
}
