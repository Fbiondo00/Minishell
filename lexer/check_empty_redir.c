/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_empty_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:12:38 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:30:37 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_is_valid_norm(char *chars, char c, int idx, int count)
{
	if (count == 0 || ft_strchr(chars, c))
		return (0);
	else
		return (idx);
}

void	norm_is_value(char *tmp, int *count, int *idx, int q)
{
	while (++(*idx) < ft_strlen(tmp))
		if (tmp[*idx] == q)
			break ;
	(*count)++;
}

// riceve idx del char dopo redir
// torna 0 se assente, quindi errore(assenza )
// torna index dell ultimo char se ok
int	is_value(t_shell *shell, char *tmp, int idx)
{
	int		count;
	char	*chars;

	count = 0;
	chars = "><()";
	while (tmp[idx] == ' ')
		idx++;
	while (idx < ft_strlen(tmp))
	{
		if (tmp[idx] == 34 && !in_quotes_str(shell, idx))
			norm_is_value(tmp, &count, &idx, 34);
		if (tmp[idx] == 39 && !in_quotes_str(shell, idx))
			norm_is_value(tmp, &count, &idx, 39);
		else if ((tmp[idx] != ' ' && !in_quotes_str(shell, idx))
			|| (ft_strchr(chars, tmp[idx]) && !in_quotes_str(shell, idx)))
		{
			count++;
			break ;
		}
		idx++;
	}
	return (ft_is_valid_norm(chars, tmp[idx], idx, count));
}

// echo 7>1>2
// ritorna 1 errore, 0 ok
int	empty_redir(t_shell *shell)
{
	int		i;
	char	*tmp;

	tmp = duplica(shell->rawline);
	i = ft_strlen(tmp);
	while (--i >= 0)
	{
		if ((tmp[i] == '>' || tmp[i] == '<') && !in_quotes_str(shell, i))
		{
			if (is_value(shell, tmp, i + 1))
				reset_value(tmp, i - 1, is_value(shell, tmp, i + 1));
			else
			{
				free(tmp);
				return (1);
			}
			if (is_fd(tmp, i) != 0 || is_fd(tmp, i) + 1 != i)
				reset_value(tmp, is_fd(tmp, i), i);
		}
	}
	free(tmp);
	return (0);
}

// 4 func