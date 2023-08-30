/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 15:42:52 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/30 23:55:21 by flaviobiond      ###   ########.fr       */
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
			k = 0;
			new_str = malloc(ft_strlen(str) - 1);
			new_str[ft_strlen(str) - 1] = '\0';
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
	int		j;
	int		len;
	char	*chars;
	char	*str;
	char	*new_str;
    
    // str = NULL;
	new_str = NULL;
	chars = "><()";
    // str = ft_wwew(node, str, idx, &i);
	i = idx + 1;
	while (node->raw_cmd[i] == ' ')
		i++;
	len = get_len_value(node, i);
	str = malloc(len + 1);
	str[len] = '\0';
	j = -1;
	while (i < ft_strlen(node->raw_cmd))
	{
		if ((node->raw_cmd[i] == ' ' && !in_quotes(node, i))
			|| (ft_strchr(chars, node->raw_cmd[i]) && !in_quotes(node, i)))
			break ;
		else if (node->raw_cmd[i] == 34 && !in_quotes(node, i))
		{
			node->flag = 34;
			while (++i < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[i] == 34)
					break ;
				str[++j] = node->raw_cmd[i];
			}
		}
		else if (node->raw_cmd[i] == 39 && !in_quotes(node, i))
		{
			node->flag = 39;
			while (++i < ft_strlen(node->raw_cmd))
			{
				if (node->raw_cmd[i] == 39)
					break ;
				str[++j] = node->raw_cmd[i];
			}
		}
		str[++j] = node->raw_cmd[i++];
	}
	new_str = modify_str(node, str);
	if (new_str)
		node->content.redir[num].value = new_str;
	else
		node->content.redir[num].value = str;
	set_redir_op(node, idx, num);
	set_raw_cmd_and_quote_idx(node, idx, i);
}

// ritorna il numero di operatori presenti nella raw_cmd
// soluzione: utilizza raw_cmd ed in_quotes()
int	get_rd_size(t_node *node)
{
	int	i;
	int	count_redir;

	i = -1;
	count_redir = 0;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
		{
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
				i++;
			count_redir++;
		}
		if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
		{
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
				i++;
			count_redir++;
		}
	}
	return (count_redir);
}

// i: indice del primo operatore (anche in caso di << >>)
// num: index della redir
// la logica dovrebbe essere semplice: per ogni )
// prima della redir: lvl++; (parte da 0)
// setta il lvl relativa alla redir salvata, prima o dopo set_fd?
// (cat || echo b) <test
// (echo b && (echo c)<2)>p)
void	set_lvl(t_node *node, int i, int num)
{
	node->content.redir[num].lvl = 0;
	while (--i >= 0)
	{
		if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
			break ;
		if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
			node->content.redir[num].lvl++;
	}
}

void	set_redirection(t_node *node)
{
	int	i;
	int	num;

	i = -1;
	num = 0;
	node->content.kv_size = get_rd_size(node);
	if (node->content.kv_size)
	{
		node->content.redir = malloc(sizeof(*node->content.redir)
				* node->content.kv_size);
		while (++i < ft_strlen(node->raw_cmd))
		{
			if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
			{
				set_lvl(node, i, num);
				set_fd(node, i, num);
				if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
					i++;
				set_token_redirection(node, i, num++);
			}
			else if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
			{
				set_lvl(node, i, num);
				set_fd(node, i, num);
				if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
					i++;
				set_token_redirection(node, i, num++);
			}
		}
	}
	else
		printf("NO REDIRECTION TO DO!\n");
}

// 5 func
