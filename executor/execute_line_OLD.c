/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line_OLD.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/05 23:34:16 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_node(t_shell *shell, t_node *node)
{
	int	i;

	i = -1;
	if (shell && shell->tree->content.idx_op != -1)
		printf("\n\nBASH:%s\n", shell->rawline);
	if (shell && shell->tree->content.idx_op == -1)
		printf("\n\nBASH:%s\n", shell->str);
	printf("---------------------------\n");
	printf("----------|NODO|-----------\n");
	printf(">node:%p\n", node);
	while (node->content.cmd[++i])
		printf("cmd[%d]:%s ", i, node->content.cmd[i]);
	printf("is_last:%d\n", node->is_last);
	printf("\n");
	printf("-----|REDIRECTION|-----\n");
	if (node->content.redir)
	{
		i = -1;
		printf("node->content.kv_size:%d\n", node->content.kv_size);
		while (++i < node->content.kv_size)
		{
			if (!i)
				printf("-----------------------\n");
			printf("REDIR N°%d\n", i + 1);
			printf("FD:%d| ", node->content.redir[i].fd);
			printf("LVL:%d| ", node->content.redir[i].lvl);
			if (node->content.redir[i].key == 13)
				printf("KEY: R_INPUT| ");
			if (node->content.redir[i].key == 14)
				printf("KEY: R_INPUT_HERE_DOC| ");
			if (node->content.redir[i].key == 15)
				printf("KEY: R_OUTPUT_TRUNC| ");
			if (node->content.redir[i].key == 16)
				printf("KEY: R_OUTPUT_APPEND| ");
			printf("VALUE:{%s}len:%d\n", node->content.redir[i].value,
					ft_strlen(node->content.redir[i].value));
		}
	}
	if (!node->content.redir)
		printf("il nodo non ha redirezioni!\n");
	if (node->back)
	{
		printf("-------|back node|-------\n");
		printf("indirizzo:%p\n", node->back);
		if (node->back->content.op == 10)
			printf("operatore: AND\n");
		if (node->back->content.op == 11)
			printf("operatore: OR\n");
		if (node->back->content.op == 12)
			printf("operatore: PIPE\n");
		printf("lvl_subshell:%d\n", node->back->lvl_subshell);
		if (node->back->back)
		{
			printf("----|back>back node|----\n");
			printf("indirizzo:%p\n", node->back->back);
			if (node->back->back->content.op == 10)
				printf("operatore: AND\n");
			if (node->back->back->content.op == 11)
				printf("operatore: OR\n");
			if (node->back->back->content.op == 12)
				printf("operatore: PIPE\n");
			printf("lvl_subshell:%d\n", node->back->back->lvl_subshell);
		}
		if (!node->back->back)
			printf("node->back->back:(null)!!\n");
	}
	if (!node->back)
		printf("node->back:(null)!!\n");
	printf("------|==FINE_NODO==|------\n");
	printf("---------------------------\n");
}