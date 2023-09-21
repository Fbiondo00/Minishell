/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:47:03 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/20 22:10:28 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// reset fd originali(con dup2) salvati in shell
void	ft_reset_original_fd(t_node *node)
{
	dup2(node->shell->temp_input, STDIN_FILENO);
	dup2(node->shell->temp_output, STDOUT_FILENO);
	dup2(node->shell->temp_error, STDERR_FILENO);
}

int	norm_if1_sub(t_node **node, int i)
{
	if (ft_open_file(*node, i) == 0)
		return (0);
	(*node)->content.redir[i].key = R_OUTPUT_APPEND;
	if (ft_open_file(*node, i) == 0)
		return (0);
	return (1);
}

// V2 is_first viene determinato dentro la funzione
// il nodo di input è il nodo storage, ovvero un nodo non attuale
// ma che conserva gli fd per il nodo attuale
// ritorna 0 errore, 1 ok
int	ft_fd_sub_level2(t_node *node, int lvl, int *is_first)
{
	int	i;

	i = -1;
	while (++i < node->content.kv_size)
	{
		if (lvl > 0 && node->content.redir[i].lvl == lvl)
		{
			if ((*is_first == 0 && (node->content.redir[i].key == R_INPUT
						|| node->content.redir[i].key == R_INPUT_HERE_DOC))
				|| (node->content.redir[i].key == R_OUTPUT_APPEND
					|| node->content.redir[i].key == R_OUTPUT_TRUNC))
			{
				if (node->content.redir[i].key == R_OUTPUT_TRUNC)
					if (!norm_if1_sub(&node, i))
						return (0);
				if (ft_open_file(node, i) == 0)
					return (0);
			}
		}
		if (node->content.redir[i].key == R_INPUT
			|| node->content.redir[i].key == R_INPUT_HERE_DOC)
			(*is_first)++;
	}
	return (1);
}

// esegue tutte le redir con lvl 0(cmd_level)
// ritorna 1 se ok, 0 se errore
int	ft_fd_cmd_level(t_node *node)
{
	int	i;

	i = -1;
	while (++i < node->content.kv_size)
	{
		if (node->content.redir[i].lvl == 0)
			if (!ft_open_file(node, i))
				return (norm_exit_status(node, 1));
	}
	return (norm_exit_status(node, 0));
}

// ritorna 0 se ok, ritorna !0 se errore
int	norm_exit_status(t_node *node, int i)
{
	if (i == 0)
	{
		node->shell->exit_status = 0;
		return (0);
	}
	else
	{
		node->shell->exit_status = 1;
		ft_reset_original_fd(node);
		return (i);
	}
}
