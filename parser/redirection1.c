/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 22:13:46 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/04 04:07:54 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// void	set_lvl(t_node *node, int i, int num)
// {
// 	node->content.redir[num].lvl = 0;
// 	while (--i >= 0)
// 	{
// 		if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
// 			break ;
// 		if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
// 			node->content.redir[num].lvl++;
// 	}
// }

// V2 ((a )>1)>2, devono risultare tutti lvl 0
void set_lvl(t_node *node, int i, int num)
{
	node->content.redir[num].lvl = 0;
	while (--i >= 0)
	{
		if ((node->raw_cmd[i] == '|' || node->raw_cmd[i] == '&') && !in_quotes(node, i))
			break;
		if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
			node->content.redir[num].lvl--;
		if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
			node->content.redir[num].lvl++;
	}
	if (node->content.redir[num].lvl < 0)
		node->content.redir[num].lvl = 0;
}

void	set_redirection2(t_node *node, int i, int num)
{
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
		{
			set_fd(node, i, num);
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
				i++;
			set_token_redirection(node, i, num++);
		}
		else if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
		{
			set_fd(node, i, num);
			if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
				i++;
			set_token_redirection(node, i, num++);
		}
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
		set_redirection2(node, i, num);
	}
}
