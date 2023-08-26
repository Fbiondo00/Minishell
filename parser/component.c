/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   component.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/13 22:49:26 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 00:31:11 by flaviobiond      ###   ########.fr       */
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

// echo"a"b
// bash: echoab: command not found
// non mette gli spazi ma appiccica tutti i char togheter
void	remove_quotes(t_node *node)
{
	int		i;
	int		j;
	int		k;
	char	*str;

	i = -1;
	while (node->raw_cmd[++i])
	{
		if (node->raw_cmd[i] == 34 || node->raw_cmd[i] == 39)
		{
			j = -1;
			k = 0;
			str = malloc(ft_strlen(node->raw_cmd));
			str[ft_strlen(node->raw_cmd)] = '\0';
			while (++j < ft_strlen(node->raw_cmd))
			{
				if (j == i)
					k++;
				str[j] = node->raw_cmd[k++];
			}
			free(node->raw_cmd);
			node->raw_cmd = str;
			i--;
		}
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

// naviga l albero e:
// se trova nodo_cmd setta exp e fa le varie rimozioni e setta lvl sui corretti nodi op
// se trova nodo_op continua a navigare l albero
void	set_components(t_shell *shell)
{
	t_node	*node;
	int		lvl_diff;

	if (is_node_cmd(shell->tree))
	{
		calculate_lvl_diff(shell->tree);
		print_node(shell, shell->tree);
		return ;
	}
	node = go_to_starter_node(shell->tree->left);
	while (1)
	{
		lvl_diff = calculate_lvl_diff(node);
		if (is_left_branch(node))
		{
			node->back->lvl_lock = 1;
			node->back->lvl_subshell += lvl_diff;
		}
		if (!is_left_branch(node) && !node->back->back->lvl_lock)
		{
			node->back->back->lvl_lock = 1;
			node->back->back->lvl_subshell += (lvl_diff
					+ node->back->lvl_subshell);
		}
		else if (!is_left_branch(node) && node->back->back->lvl_lock)
		{
			// printf("PRIMO LOOP:NON LEFT BRANCH && LOCK\n");
			node->back->back->lvl_lock = 1;
			node->back->back->back->lvl_subshell += (lvl_diff
					+ node->back->lvl_subshell);
		}
		print_node(shell, node);
		node = next_cmd(shell, node);
		if (!node)
			break ;
	}
	node = go_to_starter_node(shell->tree->right);
	node->back->lvl_subshell = shell->tree->lvl_subshell;
	while (1)
	{
		lvl_diff = calculate_lvl_diff(node);
		if (is_left_branch(node))
		{
			node->back->lvl_lock = 1;
			node->back->lvl_subshell += lvl_diff;
		}
		if (!is_left_branch(node) && shell->tree != node->back->back
			&& shell->tree != node->back && !node->back->back->lvl_lock)
		{
			node->back->back->lvl_lock = 1;
			node->back->back->lvl_subshell += (lvl_diff
					+ node->back->lvl_subshell);
		}
		else if (!is_left_branch(node) && shell->tree != node->back
				&& shell->tree != node->back->back
				&& node->back->back->lvl_lock)
		{
			node->back->back->back->lvl_subshell += (lvl_diff
					+ node->back->lvl_subshell);
		}
		print_node(shell, node);
		node = next_cmd(shell, node);
		if (!node)
			break ;
	}
}

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
	printf("node:%p\n", node);
	while (node->content.cmd[++i])
		printf("cmd[%d]:%s ", i, node->content.cmd[i]);
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

// 4 func