/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 15:42:52 by rdolzi            #+#    #+#             */
/*   Updated: 2023/10/02 15:46:37 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// ritorna la stringa andando a troncare il char node->flag
// leaks ??
char	*modify_str(t_node *node, char *str)
{
	int		i;
	int		j;
	int		k;
	char	*new_str;

	i = -1;
	new_str = NULL;
	while (str[++i])
	{
		if (node->flag != -1 && str[i] == node->flag)
		{
			if (new_str)
				free(new_str);
			j = -1;
			new_str = mody_str(str, &k);
			while (++j < ft_strlen(str) - 1)
			{
				if (j == i)
					k++;
				if (k < ft_strlen(str))
					new_str[j] = str[k++];
			}
		}
	}
	return (new_str);
}

char	*ft_wwew(t_node *node, int idx, int *i)
{
	int		len;
	char	*str;

	*i = idx + 1;
	while (node->raw_cmd[*i] == ' ')
		(*i)++;
	len = get_len_value(node, *i);
	str = ft_calloc(len + 2, 1);
	return (str);
}

char	*set(t_node *node, char *str, int *i, int *j)
{
	node->flag = 39;
	while (++(*i) < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[*i] == 39)
			break ;
		str[++(*j)] = node->raw_cmd[*i];
	}
	return (str);
}

void	set_token(t_node *node, char *chars, char *str, int *i)
{
	int	j;

	j = -1;
	while (*i < ft_strlen(node->raw_cmd))
	{
		if ((node->raw_cmd[*i] == ' ' && !in_quotes(node, *i))
			|| (ft_strchr(chars, node->raw_cmd[*i]) && !in_quotes(node, *i)))
			break ;
		else if (node->raw_cmd[*i] == 34 && !in_quotes(node, *i))
		{
			node->flag = 34;
			while (++(*i) < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[*i] == 34)
					break ;
				str[++j] = node->raw_cmd[*i];
			}
		}
		else if (node->raw_cmd[*i] == 39 && !in_quotes(node, *i))
			str = set(node, str, i, &j);
		str[++j] = node->raw_cmd[(*i)++];
	}
}

// verifica per ogni redirection ( > >> < <<)
// se è presente e se non sta nelle quotes
// num  è 0_based: è l index dell array delle redir
// idx è indice del char della redir (il secondo se >> o <<)

// 1 >a" u"  > POST node->raw_cmd|1     u"| ...taglia male
// 1 >a" 'u">zuzu >"oooo"
// 1 >a" 'u"ii>zuzu >"oooo"
void	set_token_redirection(t_node *node, int idx, int num)
{
	int		i;
	char	*chars;
	char	*str;
	char	*new_str;

	chars = "><()";
	str = ft_wwew(node, idx, &i);
	set_token(node, chars, str, &i);
	new_str = modify_str(node, str);
	if (new_str != NULL)
	{
		node->content.redir[num].value = new_str;
		free(str);
		str = NULL;
	}
	else
		node->content.redir[num].value = str;
	set_redir_op(node, idx, num);
	set_raw_cmd_and_quote_idx(node, idx, i);
}
