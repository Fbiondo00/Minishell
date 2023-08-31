/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   component.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:49:26 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/31 21:25:54 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_lowercase_cmd(t_node *node)
{
	int	i;

	i = -1;
	while (node->content.cmd[0] && node->content.cmd[0][++i])
	{
		if (node->content.cmd[0][i] >= 65 && node->content.cmd[0][i] <= 90)
			node->content.cmd[0][i] += 32;
	}
}

// ritorna un int contenente la differenza tra parentesi "(" e quelle ")"
// sostituisce le parentesi con gli spazi
// fa anche il set_cmd
int	calculate_lvl_diff(t_node *node)
{
	int	i;
	int	count;

	i = -1;
	count = 0;
	while (++i < ft_strlen(node->raw_cmd))
	{
		if (node->raw_cmd[i] == '(' && !in_quotes(node, i))
		{
			node->raw_cmd[i] = ' ';
			count++;
		}
		if (node->raw_cmd[i] == ')' && !in_quotes(node, i))
		{
			node->raw_cmd[i] = ' ';
			count--;
		}
	}
	set_cmd(node);
	return (count);
}

// void	print_node(t_shell *shell, t_node *node)
// {
// 	int	i;

// 	i = -1;
// 	if (shell && shell->tree->content.idx_op != -1)
// 		printf("\n\nBASH:%s\n", shell->rawline);
// 	if (shell && shell->tree->content.idx_op == -1)
// 		printf("\n\nBASH:%s\n", shell->str);
// 	printf("---------------------------\n");
// 	printf("----------|NODO|-----------\n");
// 	printf(">node:%p\n", node);
// 	while (node->content.cmd[++i])
// 		printf("cmd[%d]:|%s|len:%d ", i, node->content.cmd[i],
// 				ft_strlen(node->content.cmd[i]));
// 	printf("\n");
// 	printf("-----|REDIRECTION|-----\n");
// 	if (node->content.redir)
// 	{
// 		i = -1;
// 		printf("node->content.kv_size:%d\n", node->content.kv_size);
// 		while (++i < node->content.kv_size)
// 		{
// 			if (!i)
// 				printf("-----------------------\n");
// 			printf("REDIR N°%d\n", i + 1);
// 			printf("FD:%d| ", node->content.redir[i].fd);
// 			printf("LVL:%d| ", node->content.redir[i].lvl);
// 			if (node->content.redir[i].key == 13)
// 				printf("KEY: R_INPUT| ");
// 			if (node->content.redir[i].key == 14)
// 				printf("KEY: R_INPUT_HERE_DOC| ");
// 			if (node->content.redir[i].key == 15)
// 				printf("KEY: R_OUTPUT_TRUNC| ");
// 			if (node->content.redir[i].key == 16)
// 				printf("KEY: R_OUTPUT_APPEND| ");
// 			printf("VALUE:{%s}len:%d\n", node->content.redir[i].value,
// 					ft_strlen(node->content.redir[i].value));
// 		}
// 	}
// 	if (!node->content.redir)
// 		printf("il nodo non ha redirezioni!\n");
// 	if (node->back)
// 	{
// 		printf("-------|back node|-------\n");
// 		printf("indirizzo:%p\n", node->back);
// 		if (node->back->content.op == 10)
// 			printf("operatore: AND\n");
// 		if (node->back->content.op == 11)
// 			printf("operatore: OR\n");
// 		if (node->back->content.op == 12)
// 			printf("operatore: PIPE\n");
// 		printf("lvl_subshell:%d\n", node->back->lvl_subshell);
// 		if (node->back->back)
// 		{
// 			printf("----|back>back node|----\n");
// 			printf("indirizzo:%p\n", node->back->back);
// 			if (node->back->back->content.op == 10)
// 				printf("operatore: AND\n");
// 			if (node->back->back->content.op == 11)
// 				printf("operatore: OR\n");
// 			if (node->back->back->content.op == 12)
// 				printf("operatore: PIPE\n");
// 			printf("lvl_subshell:%d\n", node->back->back->lvl_subshell);
// 		}
// 		if (!node->back->back)
// 			printf("node->back->back:(null)!!\n");
// 	}
// 	if (!node->back)
// 		printf("node->back:(null)!!\n");
// 	printf("------|==FINE_NODO==|------\n");
// 	printf("---------------------------\n");
// }

void	ft_ft(t_shell *s, t_node *n)
{
	int	l;

	while (1)
	{
		l = calculate_lvl_diff(n);
		if (is_left_branch(n))
		{
			n->back->lvl_lock = 1;
			n->back->lvl_subshell += l;
		}
		if (next_cmd2(s, n) && !is_left_branch(n) && !n->back->back->lvl_lock)
		{
			n->back->back->lvl_lock = 1;
			n->back->back->lvl_subshell += (l + n->back->lvl_subshell);
		}
		else if (next_cmd2(s, n) && !is_left_branch(n)
				&& n->back->back->lvl_lock)
		{
			n->back->back->lvl_lock = 1;
			n->back->back->back->lvl_subshell += (l + n->back->lvl_subshell);
		}
		n = next_cmd2(s, n);
		if (!n)
			break ;
	}
}

// V2 con next_cmd2
//  naviga l albero e:
//  se trova nodo_cmd setta exp e fa le varie rimozioni
// e setta lvl sui corretti nodi op
//  se trova nodo_op continua a navigare l albero
void	set_components(t_shell *shell)
{
	t_node	*node;

	if (is_node_cmd(shell->tree))
	{
		calculate_lvl_diff(shell->tree);
		return ;
	}
	node = go_to_starter_node(shell->tree->left);
	ft_ft(shell, node);
}
