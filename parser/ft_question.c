/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_question.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 22:16:47 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/03 02:11:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_input1(char *new_str, t_node *node, int y)
{
	char	*raw;
	char	*quote;

	if (new_str)
	{
		raw = modify_raw_and_quote3(node, y, new_str, 32);
		ft_free_str(&node->raw_cmd);
		node->raw_cmd = raw;
		quote = modify_raw_and_quote3(node, y, new_str, 48);
		ft_free_str(&node->quote_idx);
		node->quote_idx = quote;
	}
}

void	ft_question(t_node *node, int i)
{
	char	*buff;

	buff = NULL;
	buff = ft_itoa(node->shell->exit_status);
	ft_input1(buff, node, i);
}

void	ft_do_question(t_node *node)
{
	int	i;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if ((node->raw_cmd[i] == '$' && node->raw_cmd[i + 1] == '?')
			&& in_quotes(node, i) != -1)
			ft_question(node, i);
	}
}
