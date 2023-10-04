/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:25 by rdolzi            #+#    #+#             */
/*   Updated: 2023/10/04 13:32:20 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>
# include <stdint.h>

// MESSAGE
# define PROMPT_MSG "fbrdhell-2.0 $"

// SYNTAX ERRORS (LEXER)
// exit 1
# define INVALID_ARGS "Error: invalid arguments\n"
// exit 2
# define UNCLOSED_QUOTES_ERROR "Error: unclosed quotes\n"
// exit 3
# define SYNTAX_ERROR "Error: invalid syntax\n"

// EXECUTION ERRORS
# define ERROR_FILE_OR_PATH "No such file or directory\n"
# define CMD_NOT_FOUND "command not found\n"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

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
	int					fd;
	int					key;
	int					lvl;
	char				*value;
}						t_kv;

typedef struct s_content
{
	char				**cmd;
	int					op;
	int					idx_op;
	int					kv_size;
	t_kv				*redir;
}						t_content;

typedef struct s_node
{
	char				*raw_cmd;
	char				*quote_idx;
	int					lvl_subshell;
	int					lvl_lock;
	int					done_lock;
	int					flag;
	int					flag_pipe;
	int					is_last;
	int					std_in;
	int					std_out;
	int					std_err;
	struct s_content	content;
	struct s_node		*back;
	struct s_node		*right;
	struct s_node		*left;
	struct s_shell		*shell;
}						t_node;

typedef struct s_shell
{
	int					exit_builtin;
	int					exit_status;
	int					lvl_subshell;
	struct sigaction	signal_nothing;
	struct sigaction	signal_int;
	struct sigaction	signal_quit;
	struct termios		tty_attrs;
	int					error;
	int					temp_input;
	int					temp_output;
	int					new_temp_output;
	int					new_temp_input;
	int					temp_error;
	char				*rawline;
	char				*quote_idx;
	char				**env;
	char				*str;
	t_node				*tree;
}						t_shell;

// extern void rl_clear_history(void);
// extern	void	rl_replace_line(const char *text, int clear_undo);

// LEXER
void					ft_read_line(t_shell *shell);
int						unclosed_quotes(t_shell *shell);
int						in_quotes(t_node *node, int index);
int						in_quotes_str(t_shell *shell, int index);
char					*duplica(char *str);
int						redir_after(t_shell *shell, int idx);
// after_before.c
int						op_after(t_shell *shell, int idx);
int						char_after(t_shell *shell, int idx, int flag, char *op);
int						op_before(t_shell *shell, int idx);
int						char_before(t_shell *shell, int idx);
// parentheses.c
int						check_parentheses(t_shell *shell);
int						empty_parentheses(t_shell *shell, int idx);
int						get_end_parentheses(t_shell *shell, int idx);
void					remove_par(t_shell *shell, int idx);
// recursive.c
int						recursive(t_shell *shell, int par, int left_p, int idx);
// check_empty_redir.c
int						empty_redir(t_shell *shell);
// check_empty_redir2.c
void					reset_value(char *tmp, int start, int end);
int						is_fd(char *tmp, int idx);
// read_line2.c
int						unclosed_quotes(t_shell *shell);
int						get_idx_after_redir(t_shell *shell, int idx, int flag);
int						ft_isalpha(int c);

// PARSER
int						check_op_logic_than_pipe(t_node *node);
int						is_last_char(t_node *node, int idx);
int						get_op(t_node *node, int idx);
void					set_redirection(t_node *node);
void					set_tree(t_shell *shell);
void					set_content(t_node *node);
void					set_raw_cmd_and_quote_idx(t_node *node, int start,
							int finish);
int						get_len_value(t_node *node, int idx);
void					set_components(t_shell *shell);
void					set_cmd(t_node *node);
void					ft_lowercase_cmd(t_node *node);
char					*var_expand(t_node *node, char *str);
char					*modify_raw_and_quote2(t_node *node, int idx, char *str,
							char c);
int						ft_max(t_node *node, int i);
int						ft_min(t_node *node, int i);
int						ft_stronly(const char *str, char c);
char					*ft_one(void);
char					*new_str_1(char *str, char *new_str, int flag);
int						flag1(char *entry, char *str, int i);
int						ft_middle(char *str, int index);
int						check_left(char *str, int i);
void					ft_do_cmd(t_node *node);
int						ft_get_len_env(t_shell *shell);
char					*modify_raw_and_quote(t_node *node, int idx, char *str,
							char c);
void					ft_do_question(t_node *node);
int						tot_len_1(t_node *node, char *str, int idx, int y);
char					*modify_raw_and_quote3(t_node *node, int idx, char *str,
							char c);
char					*mody_str(char *str, int *k);
void					set_redir_op(t_node *node, int idx, int num);
void					set_fd(t_node *node, int idx, int num);
void					set_token_redirection(t_node *node, int idx, int num);
int						get_rd_size(t_node *node);
void					set_lvl(t_node *node, int i, int num);
void					convert_space(t_node *node, char c);
// init
void					shell_init(int argc, char **argv, char **env,
							t_shell *shell);
void					node_init(t_node *node);
// SIGNAL
void					ft_signals(t_shell *shell);
void					ft_reset_signal(void);
void					ft_head(int sign);

// EXECUTOR
void					executev2(t_shell *shell);
t_node					*fd_storage(t_node *node);
// navigation
t_node					*go_to_starter_node(t_node *node);
int						is_left_branch(t_node *node);
int						is_node_cmd(t_node *node);
t_node					*next_cmd2(t_shell *shell, t_node *node);
t_node					*next_cmd_same_lvl(t_node *node);
// bultins
void					ft_name_value(t_node *node, t_shell *shell, int ij,
							int y);
void					ft_echo(t_node *node);
void					ft_cd(t_node *node, t_shell *shell);
void					ft_env(t_shell *shell);
void					ft_export(t_shell *shell, t_node *node);
void					ft_unset(t_node *node, t_shell *shell);
int						ft_exit(t_node *node, t_shell *shell);
void					swap_(char **str1, char **str2);
void					mall_env(t_shell *shell, char **env);
void					free_envp(t_shell *shell);
void					ft_reset_original_fd(t_node *node);
void					ft_wild(t_node *node, int i, int y);
void					ft_conc(t_shell *shell, char *str, int y);
void					bubble_sort_strings(char **strings, int num_strings);
int						ft_get_equ(char *env);
int						ft_check(t_node *node, int i);
void					ft_pwd(t_node *node);
// execve
void					ft_execve(t_node *node);
void					free_matrix(char **matrix);
// fd
int						ft_do_redir2(t_node *node);
int						ft_do_redir2_pipe(t_node *node);
int						ft_do_redir3(t_node *node);
void					ft_reset_original_fd(t_node *node);
int						norm_exit_status(t_node *node, int i);
int						ft_fd_sub_level2(t_node *node, int lvl, int *is_first);
int						ft_fd_cmd_level(t_node *node);
// open_file
int						ft_open_file(t_node *node, int i);
void					ft_do_heredoc(t_node *node);
void					ft_remove_heredoc(t_node *node);
int						ft_open(int *fd, char *str, int key);
// execute_utils.c
int						norm_check(t_node *node);
char					*sign_str(t_node *node, int lvl);
t_node					*last_cmd_same_lvl(t_node *node);
int						is_redir_out(t_node *node, int lvl);
// execute_utils2.c
t_node					*fd_storage(t_node *node);
t_node					*ft_do_pipe(t_node *node);
t_node					*ft_do_and_or(t_node *node, t_node *prev_node);
int						ft_single_cmd2(t_node *node, int (*f)(t_node *));

// UTILS
int						ft_reach(t_shell *shell, char *str);
int						ft_strlen(char *s);
char					*ft_strchr(char *s, char ch);
void					ft_clean_exit(t_shell *shell, char *str,
							int exit_status, int to_exit);
char					**ft_split(char *s, char c);
char					*ft_substr(char *s, int start, int len);
int						ft_strncmp2(char *s1, char *s2, int n, int flag);
char					ft_strcmp(char *s1, char *s2);
void					*ft_realloc(void *ptr, size_t size);
char					*ft_strdup(char *src);
char					*ft_strjoin(char const *s1, char const *s2);
void					welcome(void);
int						get_idx_eq(t_node *node, int idx);
int						get_idx_eq_str2(char *str);
int						ft_isdigit2(int c);
int						ft_get_len_mat(t_node *node);
int						ft_atoi(const char *str);
int						ft_strncmp(char *s1, char *s2, int n, int flag);
int						get_idx_eq_str(char *str);
char					*ft_strjoin2(char const *s1, char const *s2);
char					*ft_itoa(int n);
void					*ft_calloc(size_t count, size_t size);
void					ft_bzero(void *s, size_t n);
// exec_utils
int						ft_dup2(int *fd, int arg);
int						ok_status(t_node *node);
int						is_builtin(t_node *node);
t_node					*go_next_cmd_and_or(t_node *node);
int						execute_builtin(t_node *node, t_shell *shell);
void					execute_recursivev2(t_node *node);

// GNL
char					*get_next_line(int fd);
char					*ft_strjoin_gnl(char *s1, char *s2);
int						ft_strchr_gnl(char *s1, char c);
size_t					ft_strlen_gnl(char *str);
void					*ft_free(char *s1, char *s2);
size_t					ft_is_newline(char c);
// clean_exit.c
void					ft_free_str(char **str);
// execute_single_cmd.c
void					execute_single_cmd(t_node *node);
int						ft_get_op(t_node *node);
t_node					*ft_back_node(t_node *node);
t_node					*next_onev2(t_node *node);
int						norm_checkv2skip(t_node *node);
t_node					*wrap_ft_single_cmd2(t_node *node);

//last adds
char					*norm_wild_return(char **dirname, DIR **dir,
							char *new_str, int flag);
void					norm_wild(char **new_str, char *tem);
void					ft_clean_exit1(t_shell *shell, int exit_status);
void					norm_remove_heredoc(t_shell *shell);
void					remove_node(t_node *node);
void					just_reset(t_shell *shell);
int						ft_get_op(t_node *node);
t_node					*ft_back_node(t_node *node);
void					fix_out_trunc(t_node *node);
t_node					*wrap_ft_single_cmd2(t_node *node);
t_node					*just_executev2(t_node *temp);
t_node					*fork_executev2(t_node *node);
int						get_max_lvl(t_node *node, t_node *stg);
void					norm_node(t_node **node, t_node **prev_node,
							t_node **next_node);
void					norm_else_single_cmd(t_node **node, int *status,
							int pid);
t_node					*do_exit(t_node *node);
int						norm_check2(t_node *node);
int						norm_while_here(t_node *node, char *str, int i);
void					norm_here_doc1(t_node *node, char **str, int i, int fd);

extern int				g_flag_status;

#endif