/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 17:05:04 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/30 17:06:14 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// converte gli spazi dentro le quotes in c
void	convert_space(t_node *node, char c)
{
	int	i;

	i = -1;
	while (++i < ft_strlen(node->raw_cmd))
		if (node->raw_cmd[i] == ' ' && in_quotes(node, i))
			node->raw_cmd[i] = c;
}
