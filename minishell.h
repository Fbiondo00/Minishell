/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:25 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/13 01:24:40 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>

// MESSAGE
# define PROMPT_MSG "fbrdhell-0.1 $"
# define UNCLOSED_QUOTES_ERROR "Error: unclosed quotes\n"

//EXECUTOR OPERATOR
# define AND 10
# define OR 11
# define PIPE 12

// KEY
// <
# define R_INPUT 13
// <<
# define R_INPUT_HERE_DOC 14
// >
# define R_OUTPUT_TRUNC 15
// >>
# define R_OUTPUT_APPEND 16


// key: tipologia redirect, value: nome file
typedef struct s_kv
{
	int		key;
	char	*value;
}	t_kv;

typedef struct s_content
{
	char	**cmd;
	int		op;
	int		kv_size;
	t_kv	*redir;
}	t_content;

typedef struct s_node
{
	char				*raw_cmd;
	struct s_content	content;
	struct s_node		*back;
	struct s_node		*right;
	struct s_node		*left;
}	t_node;

typedef struct s_shell
{
	int		is_alive;
	int		temp_input;
	int		temp_output;
	int		temp_error;
	char	*rawline;
	char	*quote_idx;
	char	**env;
	t_node	*tree;
}	t_shell;

// LEXER
void	ft_read_line(t_shell *shell);
int		unclosed_quotes(t_shell *shell);
int		in_quotes(t_shell *shell, int index);

// PARSER
void	set_tree(t_shell *shell);
int		get_idx_next_op(t_shell *shell, int idx, int dir);
int		check_op_logic_than_pipe(t_shell *shell, int idx_start, int dir);
int		get_op(t_shell *shell, int idx);
void	set_content(t_shell *shell, t_node *node, int operator);
void	set_redirection(t_shell *shell, t_node *node);

//testing purposes
void	print_str(char *str);
void print_node(t_node *node);

// utils
int		ft_strlen(char *s);
char	*ft_strchr(char *s, char ch);
void	ft_exit(t_shell *shell, char *str);
int		is_last_char(t_shell *shell, int idx); //cambiare in (char *, int)
#endif