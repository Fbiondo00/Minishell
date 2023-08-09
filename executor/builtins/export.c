/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:19:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/09 01:38:30 by flaviobiond      ###   ########.fr       */
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
	for (int i = 0; i < num_strings - 1; i++)
	{
		for (int j = 0; j < num_strings - i - 1; j++)
		{
			if (ft_strcmp(strings[j], strings[j + 1]) > 0)
			{
				swap_(&strings[j], &strings[j + 1]);
			}
		}
	}
}

int    ft_reach(t_shell *shell, char *str)
{
    int i;
    int y;
    // int ij;
    
    // ij = 0;
    y = -1;
    i = -1;
    while(str[++y] != '=' && str[y])
        ;
    printf("caratteri per ogni stringa:%d\n", y);
    // return ;
    while(shell->env[++i] != NULL)
        {
            if((ft_strncmp(shell->env[i], str, y)) == 0)
            {          
                if(str[y] == '=')
                {
                    printf("i:%d\nenv[i]:%c\ns",i ,str[y]);
                    free(shell->env[i]);
                    printf("i:%d\nenv[i]:%s\ns",i ,shell->env[i]);
                    shell->env[i] = str;
                      printf("str:%s|env[i]:%s\n%d\n", str, shell->env[i], y);
                    return(1);
                }
                return(1);
             //   printf("str:%s|env[i]:%s\n%d\n", str, shell->env[i], y);
                
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

void	ft_export(t_shell *shell, t_node *node)
{
	int	i;
	int	y;
	int	ij;

	y = 0;
	ij = ft_get_len_mat(node);
	if (ij >= 2)
	{
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
