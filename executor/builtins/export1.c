/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:59:32 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/17 18:08:27 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
	int	i;
	int	j;

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

void	ft_conc_2(t_shell *shell, char *str, int i)
{
	int		y;
	char	*str1;

	y = -1;
	str1 = malloc(ft_strlen(str));
	if (!str1)
		return (exit(1));
	while (str[++y] != '+')
		str1[y] = str[y];
	while (str[++y] != '+' && str[y - 1])
		str1[y - 1] = str[y];
	shell->env = ft_realloc(shell->env, sizeof(char *) * (i + 2));
	shell->env[i] = str1;
	shell->env[i + 1] = 0;
}

void	ft_conc(t_shell *shell, char *str, int y)
{
	int		j;
	int		i;
	char	*new_str;

	i = -1;
	while (shell->env[++i])
	{
		if ((ft_strncmp(shell->env[i], str, y, 1)) == 0)
		{
			j = -1;
			while (str[++j])
			{
				if (str[j] == '=')
					break ;
			}
			new_str = ft_strjoin(shell->env[i], str + ++j);
			free(shell->env[i]);
			shell->env[i] = new_str;
			return ;
		}
	}
	ft_conc_2(shell, str, i);
}

int	ft_get_equ(char *env)
{
	int	i;

	i = -1;
	while (env[++i] && env[i] != '=')
		;
	return (i);
}
