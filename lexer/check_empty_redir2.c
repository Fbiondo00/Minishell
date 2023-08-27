/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_empty_redir2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:19:07 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:30:39 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*duplica(char *str)
{
	int		i;
	char	*new;

	i = -1;
	new = malloc(ft_strlen(str));
	while (++i < ft_strlen(str))
		new[i] = str[i];
	new[ft_strlen(str)] = '\0';
	return (new);
}

// controlla se è presente fd valido prima della redir
// ritorna 0 se non è fd valido
// ritorna idx del primo spazio dopo il char di fd
int	is_fd(char *tmp, int idx)
{
	char	*chars;

	chars = "><()";
	if (tmp[idx - 1] == '>' || tmp[idx - 1] == '<')
		idx--;
	while (--idx > 0)
	{
		if (tmp[idx] == ' ')
			break ;
		if (ft_strchr(chars, tmp[idx]))
			return (idx);
		if (!ft_isdigit2(tmp[idx]))
			return (0);
	}
	return (idx);
}

// mette gli spazi a posto dei char elaborati
// start: index di < o > (se << o >>, si tratta dell ultimo dei due)
// end: index dell ultimo char del value
void	reset_value(char *tmp, int start, int end)
{
	int	i;

	i = start + 1;
	while (i <= end)
		tmp[i++] = ' ';
}

// 3 func