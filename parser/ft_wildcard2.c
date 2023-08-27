/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 15:20:21 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/27 04:11:10 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_input(char *new_str, t_node *node, int y)
{
	char	*raw;
	char	*quote;

	if (new_str)
	{
		printf("ft_wildfinalestr2:%s\n", new_str);
		raw = modify_raw_and_quote2(node, y, new_str, 32);
		// free(node->raw_cmd);
		node->raw_cmd = raw;
		quote = modify_raw_and_quote2(node, y, new_str, 48);
		//    free(node->quote_idx);
		node->quote_idx = quote;
	}
}

char	*create_str(t_node *node, int max, int min)
{
	char	*str;
	int		ij;

	ij = 0;
	str = malloc(max - min + 1);
	if (!str)
		return (NULL);
	while (min <= max && (node->raw_cmd[min] != 32 && node->raw_cmd[min]))
		str[ij++] = node->raw_cmd[min++];
	str[ij++] = 0;
	printf("ft_wild_str:%s|str[i:%d]:\n", str, ft_strlen(str));
	return (str);
}

void	ft_wild(t_node *node, int i, int y)
{
	int		max;
	int		min;
	char	*str;
	char	*new_str;

	new_str = NULL;
	max = ft_max(node, i);
	printf("\nft_wildmax:%d\n", max);
	min = ft_min(node, i);
	if (min < 0)
		min = 0;
	printf("\nft_wildmin: %d\n", min);
	str = create_str(node, max, min);
	if (ft_stronly(str, '*') == 1)
		new_str = ft_one();
	else
		new_str = new_str_1(str, new_str, 0);
	free(str);
	ft_input(new_str, node, y);
}
