/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:25 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/17 15:14:04 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <termios.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <string.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>

// MESSAGE
# define PROMPT_MSG "fbrdhell-2.0 $"
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
	int		idx_op; // needed ?
	int		kv_size;
	t_kv	*redir;
}	t_content;

typedef struct s_node
{
	char				*raw_cmd; //operator
	char				*quote_idx; //operator
	struct s_content	content;
	struct s_node		*back;
	struct s_node		*right;
	struct s_node		*left;
}	t_node;

typedef struct s_shell
{
	struct sigaction   signal_nothing;
	struct sigaction	signal_int;
	struct sigaction	signal_quit;
	struct termios		tty_attrs;
	int					error;
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
int in_quotes(t_node *node, int index);

// PARSER
int check_op_logic_than_pipe(t_node *node);
int is_last_char(t_node *node, int idx);
int get_op(t_node *node, int idx);
void set_redirection(t_node *node);
void	set_tree(t_shell *shell);
void set_content(t_node *node);
void set_raw_cmd_and_quote_idx(t_node *node, int start, int finish);
int get_len_value(t_node *node, int idx);

//testing purposes
void	print_str(char *str);
void print_node(t_node *node);

// utils
int		ft_strlen(char *s);
char	*ft_strchr(char *s, char ch);
void	ft_exit(t_shell *shell, char *str);

// signal
void	ft_signals(t_shell *shell);
#endif