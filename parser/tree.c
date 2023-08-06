/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:51:56 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/16 22:17:59 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	set_node_operator(t_node *node, t_node *back_node, int idx_start);
void	set_node_cmd(t_node *node, t_node *back_node);

char	*get_raw_string(char *str, int idx, int dir)
{
	int		i;
	int		diff;
	int		len;
	char	*new_str;

	i = -1;
	if (dir == 1)
	{
		len = ft_strlen(str);
		diff = len - idx;
		new_str = malloc(diff);
		new_str[diff - 1] = '\0';
		while (++i < diff - 1)
			new_str[i] = str[++idx];
	}
	else
	{
		diff = idx - 0;
		new_str = malloc(diff + 1);
		new_str[diff] = '\0';
		while (++i < diff)
			new_str[i] = str[i];
	}
	return (new_str);
}


void	set_node_cmd(t_node *node, t_node *back_node)
{
	node->back = back_node;
	node->left = NULL;
	node->right = NULL;
	node->content.op = 0;
	node->content.idx_op = -1;
	printf("In child node_cmd, raw_cmd{%s}\n", node->raw_cmd);
	printf("set_content..\n");
	set_content(node);
}

void set_branch(t_node *node, t_node *back_node)
{
	int new_idx;

	new_idx = check_op_logic_than_pipe(node);
	printf("in branch> next_op index: %d\n", new_idx);
	if (new_idx)
	{
		printf("in set_branch >set_node_operator..\n");
		set_node_operator(node, back_node, new_idx);
	}
	else
	{
		printf("in set_branch >set_node_cmd..\n");
		set_node_cmd(node, back_node);
	}
}

void	set_node_operator(t_node *node, t_node *back_node, int idx_start)
{
	t_node *left_node;
	t_node *right_node;

	// malloc/setup: left & right node
	left_node = malloc(sizeof(*left_node));
	right_node = malloc(sizeof(*right_node));
	node->left = left_node;
	node->right = right_node;
	
	// set back_node
	node->back = back_node;

	// set op value and idx
	node->content.op = get_op(node, idx_start);
	node->content.idx_op = idx_start; // strategia iniziale...serve ancora?

	// print stuff...(debug)
	printf("idx_start: %d\n", idx_start);
	printf("operator: %d\n", node->content.op);

	// set raw_cmd & quote_idx in left & right node (da usare in set_branch)
	node->left->raw_cmd = get_raw_string(node->raw_cmd, idx_start, -1);
	node->left->quote_idx = get_raw_string(node->quote_idx, idx_start, -1);
	if (node->content.op == 10 || node->content.op == 11)
		idx_start++;
	printf("idx_start:%d\n", idx_start);
	node->right->raw_cmd = get_raw_string(node->raw_cmd, idx_start, 1);
	node->right->quote_idx = get_raw_string(node->quote_idx, idx_start, 1);

	// print stuff...(debug)
	printf("ACTUAL raw_cmd{%s}  quote_idx{%s}\n", node->raw_cmd, node->quote_idx);
	printf("LEFT raw_cmd{%s} quote_idx{%s}\n", node->left->raw_cmd, node->left->quote_idx);
	printf("RIGHT raw_cmd{%s} quote_idx{%s}\n", node->right->raw_cmd, node->right->quote_idx);

	// build up: left & right node
	printf("set_branch: LEFT..\n");
	set_branch(node->left, node);
	printf("set_branch: RIGHT..\n");
	set_branch(node->right, node);
}

// V2
void set_tree(t_shell *shell)
{
	t_node *node;
	int idx_start;
	
	// --init and set starter node ---
	node = malloc(sizeof(*node));
	node->raw_cmd = shell->rawline;
	node->quote_idx = shell->quote_idx;
	shell->tree = node;
	// -------------------------
	idx_start = check_op_logic_than_pipe(node);
	printf("check_op_logic_than_pipe, index:%d\n", idx_start);
	if (idx_start)
	{
		printf("set_node_operator..\n");
		set_node_operator(node, NULL, idx_start);
	}
	else
	{
		printf("set_node_cmd..\n");
		printf("ACTUAL raw_cmd{%s}  quote_idx{%s}\n", node->raw_cmd, node->quote_idx);
		set_node_cmd(node, NULL);
	}
}
