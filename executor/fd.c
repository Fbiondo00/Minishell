/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:47:03 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/25 18:53:16 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_reset_original_fd(t_node *node)
{
	dup2(node->shell->temp_input, STDIN_FILENO);
	dup2(node->shell->temp_output, STDOUT_FILENO);
	dup2(node->shell->temp_error, STDERR_FILENO);
}

int	ft_fd_sub_level(t_node *node, int lvl, int is_first)
{
	int	i;

	i = -1;
	printf("lvl:%d\n", lvl);
	printf("in ft_fd_sub_level...\n");
	while (++i < node->content.kv_size)
	{
		if (node->content.redir[i].lvl == lvl && (is_first == 0
				&& (node->content.redir[i].key == R_INPUT
					|| node->content.redir[i].key == R_INPUT_HERE_DOC)))
		{
			printf("ft_fd_sub_level: FAft_open_file!\n");
			if (!ft_open_file(node, i))
				return (0);
		}
		else
		{
			if (!ft_open_file(node, i))
				return (0);
		}
	}
	return (1);
}

int	ft_fd_cmd_level(t_node *node)
{
	int	i;

	i = -1;
	while (++i < node->content.kv_size)
	{
		if (node->content.redir[i].lvl == 0)
		{
			if (!ft_open_file(node, i))
				return (0);
		}
	}
	return (1);
}

int	ft_do_redir(t_node *node)
{
	int	flag;
	int	max_lvl;

	flag = 0;
	if (node->content.redir)
		max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
	else
		max_lvl = 0;
	while (max_lvl > 0)
	{
		if (!ft_fd_sub_level(node, max_lvl--, flag))
		{
			ft_reset_original_fd(node);
			return (0);
		}
		flag++;
	}
	if (!ft_fd_cmd_level(node))
	{
		ft_reset_original_fd(node);
		return (0);
	}
	return (1);
}
