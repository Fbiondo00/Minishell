/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:55:33 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/26 13:55:02 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*var_expand(t_node *node, char *str)
{
	int		i;
	char	*new_str;

	i = -1;
	new_str = NULL;
	printf("IN var_expand\n");
	while (node->shell->env[++i])
	{
		if (!ft_strncmp2(str, node->shell->env[i], get_idx_eq(node, i), 1))
		{
			new_str = node->shell->env[i] + get_idx_eq(node, i) + 1;
			break ;
		}
	}
	if (new_str)
		printf("value of%s is %s\n", node->shell->env[i], new_str);
	if (!new_str)
		printf("NEW_STR e' NULL\n");
	free(str);
	return (new_str);
}

// idx corrisponde a index di $
// ritorna la stringa della variabile
// ritorna null se non c e' alcun char attaccato a seguire
char	*find_var(t_node *node, int idx)
{
	int		i;
	int		y;
	int		flag;
	char	*str;

	flag = 0;
	y = idx + 1;
	while (y < ft_strlen(node->raw_cmd) && node->raw_cmd[y] != ' '
		&& node->raw_cmd[y] != 34 && node->raw_cmd[y] != 39
		&& node->raw_cmd[y] != '$' && node->raw_cmd[y] != '/')
	{
		y++;
		flag++;
	}
	if (flag == 0)
		return (NULL);
	str = malloc(flag + 1);
	str[flag] = '\0';
	i = -1;
	while (flag-- > 0)
		str[++i] = node->raw_cmd[++idx];
	printf("VARIABILE:%s\n", str);
	return (var_expand(node, str));
}

void	ft_do_expand(t_node *node)
{
	int		i;
	char	*raw;
	char	*quote;
	char	*str;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '$' && in_quotes(node, i) != -1)
		{
			str = find_var(node, i);
			raw = modify_raw_and_quote(node, i, str, 32);
			// free(node->raw_cmd);
			node->raw_cmd = raw;
			quote = modify_raw_and_quote(node, i, str, 48);
			// free(node->quote_idx);
			node->quote_idx = quote;
		}
	}
}

void	ft_do_asterisk(t_node *node)
{
	int	i;
	int	y;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '*' && !in_quotes(node, i))
		{
			y = i;
			while (node->raw_cmd[y])
			{
				if (node->raw_cmd[y] == ' ')
					break ;
				y--;
			}
			y++;
			ft_wild(node, i, y);
		}
	}
}

// elabora la stringa e produce l array di stringhe
// da dare in pasto all EXECVE
// tuttavia deve ignorare le redirection che potrebbero essere d intralcio
// in quanto possono capitare in mezzo al comando.
// probabile questo problema debba esser risolto in set_redirection
// che rielabora nuovamente la raw_cmd andando a fare il trim dei char che
// ha usato per la redirection
// NUOVO: ho aggiunto attributo shell al nodo
void	set_cmd(t_node *node)
{
	ft_do_asterisk(node);
	ft_do_expand(node);
	remove_quotes(node);
	node->content.cmd = ft_split(node->raw_cmd, ' ');
	ft_lowercase_cmd(node);
}
