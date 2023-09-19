/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_cmd1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 02:23:15 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/19 15:16:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_single_cmd(t_node *node)
{
	ft_do_heredoc(node);
	ft_single_cmd2(node, ft_do_redir3);
}

// ritorna 1 se presente
int	lvl_is_present(t_node *stg, int lvl)
{
	int	i;

	i = -1;
	while (++i < stg->content.kv_size)
		if (stg->content.redir[i].lvl == lvl)
			return (1);
	return (0);
}

int	norm_redir2(t_node *node, int status)
{
	ft_reset_original_fd(node);
	return (norm_exit_status(node, status));
}

int	norm_if_redir2(t_node *node, t_node *storage, int *mx_lv, int *flag)
{
	(*mx_lv)--;
	while (*mx_lv <= node->shell->lvl_subshell)
	{
		if (!lvl_is_present(storage, *mx_lv))
			(*mx_lv)++;
		else
			break ;
	}
	if (storage->content.redir && node->shell->lvl_subshell > 0)
	{
		*flag = 0;
		if (ft_fd_sub_level2(storage, *mx_lv, flag) == 0)
			return (norm_redir2(node, 2));
	}
	return (0);
}

// V2 utilizzare solo per multi_cmd
// esegue in ordine:
// 2. ft_fd_sub_level
// 3. ft_fd_cmd_level
// se ok setta tali valori in std_...
// se ko fa il reset dei valori originali std_..
// return 2 se ko sub_lvl
// return 1 se ko cmd_lvl
// return 0 se tutto ok
int	ft_do_redir2(t_node *node)
{
	int		flag;
	int		mx_lv;
	int		res;
	t_node	*storage;

	flag = 0;
	storage = fd_storage(node);
	mx_lv = get_max_lvl(node, storage);
	if (node->shell->lvl_subshell > 1 && mx_lv >= 1)
	{
		res = (norm_if_redir2(node, storage, &mx_lv, &flag));
		if (res == 2)
			return (2);
	}
	else
	{
		if (storage->content.redir && node->shell->lvl_subshell > 0)
			if (ft_fd_sub_level2(storage, mx_lv, &flag) == 0)
				return (norm_redir2(node, 2));
	}
	if (ft_fd_cmd_level(node) == 1)
		return (norm_redir2(node, 1));
	return (norm_exit_status(node, 0));
}
