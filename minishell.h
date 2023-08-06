/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:25 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/06 17:52:55 by flaviobiond      ###   ########.fr       */
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

#include <errno.h> // ?
#include <fcntl.h> // ?
#include <stdarg.h> // ?
#include <sys/types.h> // ?
#include <sys/wait.h>  // ?
#include <termios.h>
#include <signal.h>

// MESSAGE
# define PROMPT_MSG "fbrdhell-2.0 $"

// SYNTAX ERRORS (LEXER)
// exit 1
# define INVALID_ARGS "Error: invalid arguments\n" 
// exit 2
# define UNCLOSED_QUOTES_ERROR "Error: unclosed quotes\n"
// bash-3.2$ |echo a
// bash: syntax error near unexpected token `|'
// bash-3.2$ (ls) echo a
// bash : syntax error near unexpected token `echo'
// exit 3
#define SYNTAX_ERROR "Error: invalid syntax\n"

// EXECUTION ERRORS (EXECUTOR)
//bash-3.2$ ls /Users/ruggero/w
// ls: /Users/ruggero/w: No such file or directory
// eventualmente lasciare il placeholder per il file o path errato?
# define ERROR_FILE_OR_PATH "No such file or directory\n"
// bash-3.2$ piz u
// bash: piz: command not found
# define CMD_NOT_FOUND "command not found\n"

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
	int		exit_builtin;
	// ------
	struct	sigaction signal_nothing;
	struct	sigaction signal_int;
	struct	sigaction signal_quit;
	struct	termios tty_attrs;
	int		error;
	// ------
	int		is_alive; // needed in signals?
	int		temp_input;
	int		temp_output;
	int		temp_error;
	char	*rawline;
	char	*quote_idx;
	char	**env;
	char	*str;
	t_node	*tree;
}	t_shell;

// ?
//extern void rl_replace_line(const char *text, int clear_undo);
//extern void rl_clear_history(void);

// LEXER
void	ft_read_line(t_shell *shell);
int		unclosed_quotes(t_shell *shell);
int in_quotes(t_node *node, int index);
int in_quotes_str(char *str, int index);

// PARSER
int check_op_logic_than_pipe(t_node *node);
int is_last_char(t_node *node, int idx);
int get_op(t_node *node, int idx);
void set_redirection(t_node *node);
void	set_tree(t_shell *shell);
void set_content(t_node *node);
void set_raw_cmd_and_quote_idx(t_node *node, int start, int finish);
int get_len_value(t_node *node, int idx);

// SIGNAL
void ft_signals(t_shell *shell);
void ft_reset_signal(void);
void ft_head(int sign); //?

// EXECUTOR
void    execute(t_shell *shell);
void execute_demo(t_shell *shell); // per testare le func builtins


// UTILS
int		ft_strlen(char *s);
char	*ft_strchr(char *s, char ch);
void ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit);
char	**ft_split(char *s, char c);
char	*ft_substr(char	*s, int start, int len);
int ft_strncmp(char *s1, char *s2, int n);
char ft_strcmp(char *s1, char *s2);
void *ft_realloc(void *ptr, size_t size);
char *ft_strdup(char *src);
char *ft_strjoin(char const *s1, char const *s2);
void welcomeScreen(void);

// testing purposes
void print_str(char *str);
// void    tree_of_life(t_shell *shell);
void putstr(const char *str);

// bultin
void ft_pwd(void);
void ft_echo(t_node *node);
void ft_cd(t_node *node, t_shell *shell);
void ft_env(t_shell *shell);
void mall_env(t_shell *shell, char **env);
void ft_export(t_shell *shell);

#endif